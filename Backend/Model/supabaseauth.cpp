#include "supabaseauth.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

SupabaseAuth::SupabaseAuth(QObject *parent)
    : QObject(parent),
    m_networkManager(new QNetworkAccessManager(this)),
    m_isAuthenticated(false),
    m_isSigningUp(false),
    m_justSignedUp(false)
{
}

void SupabaseAuth::setSupabaseConfig(const QString &projectUrl, const QString &anonKey) {
    m_projectUrl = projectUrl;
    m_anonKey = anonKey;
    qDebug() << "[SupabaseAuth] Config set - URL:" << m_projectUrl;
}

//login or signup (if acc don't exist it will signup)
void SupabaseAuth::signInOrSignUp(const QString &email, const QString &password) {
    if (m_projectUrl.isEmpty() || m_anonKey.isEmpty()) {
        m_errorMessage = "Supabase configuration not set. Please set project URL and anon key.";
        emit errorOccurred();
        emit authenticationFailed(m_errorMessage);
        return;
    }

    m_pendingEmail = email;
    m_pendingPassword = password;
    m_errorMessage.clear();
    m_isSigningUp = false; // Reset flag

    // First, try to sign in
    QUrl url(m_projectUrl + "/auth/v1/token?grant_type=password");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("apikey", m_anonKey.toUtf8());
    request.setRawHeader("Authorization", ("Bearer " + m_anonKey).toUtf8());

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = m_networkManager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleSignInResponse(reply);
        reply->deleteLater();
    });
}

//catch sign in response, check for errors
void SupabaseAuth::handleSignInResponse(QNetworkReply *reply) {
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    bool hasError = (reply->error() != QNetworkReply::NoError || httpStatus >= 400);

    QByteArray responseData = reply->readAll();
    QString responseString = QString::fromUtf8(responseData);

    if (hasError) {
        qDebug() << "[SupabaseAuth] Sign in error - Status:" << httpStatus;
        qDebug() << "[SupabaseAuth] Sign in error - Response:" << responseString;
        qDebug() << "[SupabaseAuth] Sign in error - QNetworkReply error:" << reply->errorString();

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
        QJsonObject json;
        if (parseError.error == QJsonParseError::NoError) {
            json = doc.object();
        } else {
            qDebug() << "[SupabaseAuth] JSON parse error:" << parseError.errorString();
        }

        // Check if it's a "user not found" or "invalid credentials" error
        QString errorMsg = extractErrorMessage(json);
        if (errorMsg.isEmpty() && json.contains("msg")) {
            errorMsg = json["msg"].toString();
        }
        if (errorMsg.isEmpty() && json.contains("message")) {
            errorMsg = json["message"].toString();
        }

        // If sign-in fails with invalid credentials, try to sign up (user may not exist)
        if ((httpStatus == 400 || httpStatus == 401) && !m_isSigningUp && !m_justSignedUp) {
            qDebug() << "[SupabaseAuth] Sign in failed with invalid credentials, attempting sign up (user may not exist)...";
            m_isSigningUp = true;
            attemptSignUp(m_pendingEmail, m_pendingPassword);
            return;
        }

        // Reset the flag after handling the error
        m_justSignedUp = false;

        if (errorMsg.isEmpty()) {
            if (httpStatus == 401) {
                errorMsg = "Invalid email or password. Please try again.";
            } else if (httpStatus == 400) {
                errorMsg = "Invalid request. Please check your email and password.";
            } else {
                errorMsg = reply->errorString();
                if (errorMsg.isEmpty()) {
                    errorMsg = QString("Sign in failed (Status: %1)").arg(httpStatus);
                }
            }
        }

        m_errorMessage = errorMsg;
        m_isAuthenticated = false;
        m_isSigningUp = false;
        qDebug() << "[SupabaseAuth] Emitting authentication failed with message:" << m_errorMessage;
        emit errorOccurred();
        emit authenticationFailed(m_errorMessage);
        return;
    }

    // Success - parse the response get token
    qDebug() << "[SupabaseAuth] Sign in success - Response:" << responseString;
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject json = doc.object();

    m_accessToken = json["access_token"].toString();
    m_refreshToken = json["refresh_token"].toString();
    m_userId = json["user"].toObject()["id"].toString();
    m_userEmail = json["user"].toObject()["email"].toString();
    m_isAuthenticated = true;
    m_errorMessage.clear();
    m_justSignedUp = false; // Reset on successful sign-in

    qDebug() << "[SupabaseAuth] Sign in successful for user:" << m_userEmail;
    emit authenticationChanged();
    emit authenticationSucceeded(m_userId, m_userEmail);
}

//sign up response, catch errors
void SupabaseAuth::handleSignUpResponse(QNetworkReply *reply) {
    QByteArray responseData = reply->readAll();
    QString responseString = QString::fromUtf8(responseData);
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "[SupabaseAuth] Signup response status:" << httpStatus;
    qDebug() << "[SupabaseAuth] Signup response:" << responseString;

    if (reply->error() != QNetworkReply::NoError || httpStatus >= 400) {
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

        if (parseError.error == QJsonParseError::NoError) {
            QJsonObject json = doc.object();
            m_errorMessage = extractErrorMessage(json);
        }

        if (m_errorMessage.isEmpty()) {
            m_errorMessage = reply->errorString();
            if (m_errorMessage.isEmpty()) {
                m_errorMessage = QString("Signup failed with status %1").arg(httpStatus);
            }
        }

        qDebug() << "[SupabaseAuth] Signup error:" << m_errorMessage;
        m_isAuthenticated = false;
        m_isSigningUp = false;
        emit errorOccurred();
        emit authenticationFailed(m_errorMessage);
        return;
    }

    // Parse the response
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "[SupabaseAuth] JSON parse error:" << parseError.errorString();
        m_errorMessage = "Unexpected response from signup. Please try again.";
        m_isAuthenticated = false;
        m_isSigningUp = false;
        emit errorOccurred();
        emit authenticationFailed(m_errorMessage);
        return;
    }

    QJsonObject json = doc.object();

    // With email confirmation off, Supabase should return tokens directly
    if (json.contains("access_token") && json.contains("user")) {
        QJsonObject userObj = json["user"].toObject();
        m_accessToken = json["access_token"].toString();
        m_refreshToken = json["refresh_token"].toString();
        m_userId = userObj["id"].toString();
        m_userEmail = userObj["email"].toString();
        m_isAuthenticated = true;
        m_errorMessage.clear();
        m_isSigningUp = false;
        m_justSignedUp = false;

        qDebug() << "[SupabaseAuth] Sign up and login successful for user:" << m_userEmail;
        emit authenticationChanged();
        emit authenticationSucceeded(m_userId, m_userEmail);
        return;
    }

    // If no tokens, attempt sign-in (fallback)
    qDebug() << "[SupabaseAuth] Sign up successful, attempting sign in...";
    m_isSigningUp = false;
    m_justSignedUp = true;

    QUrl url(m_projectUrl + "/auth/v1/token?grant_type=password");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("apikey", m_anonKey.toUtf8());
    request.setRawHeader("Authorization", ("Bearer " + m_anonKey).toUtf8());

    QJsonObject signInJson;
    signInJson["email"] = m_pendingEmail;
    signInJson["password"] = m_pendingPassword;

    QJsonDocument signInDoc(signInJson);
    QByteArray signInData = signInDoc.toJson();

    QNetworkReply *signInReply = m_networkManager->post(request, signInData);
    connect(signInReply, &QNetworkReply::finished, this, [this, signInReply]() {
        handleSignInResponse(signInReply);
        signInReply->deleteLater();
    });
}

//attempt sign up
void SupabaseAuth::attemptSignUp(const QString &email, const QString &password) {
    QUrl url(m_projectUrl + "/auth/v1/signup");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("apikey", m_anonKey.toUtf8());
    request.setRawHeader("Authorization", ("Bearer " + m_anonKey).toUtf8());
    request.setRawHeader("Prefer", "return=representation");

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    qDebug() << "[SupabaseAuth] Attempting signup for:" << email;
    qDebug() << "[SupabaseAuth] Signup request data:" << QString::fromUtf8(data);

    QNetworkReply *reply = m_networkManager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleSignUpResponse(reply);
        reply->deleteLater();
    });
}

//sign out function
void SupabaseAuth::signOut() {
    m_accessToken.clear();
    m_refreshToken.clear();
    m_userId.clear();
    m_userEmail.clear();
    m_isAuthenticated = false;
    m_errorMessage.clear();

    emit authenticationChanged();
    qDebug() << "[SupabaseAuth] Signed out";
}

QString SupabaseAuth::extractErrorMessage(const QJsonObject &json) {
    if (json.contains("error_description")) {
        return json["error_description"].toString();
    }
    if (json.contains("message")) {
        return json["message"].toString();
    }
    if (json.contains("error")) {
        QString error = json["error"].toString();
        if (json.contains("msg")) {
            return error + ": " + json["msg"].toString();
        }
        return error;
    }
    return QString();
}

