#include "supabaseclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>


// SupabaseClient class (handles all Supabase operations)

SupabaseClient::SupabaseClient(QObject *parent)
    : QObject(parent),
    m_networkManager(new QNetworkAccessManager(this)) { }

// Set Supabase project URL and anon key (NOT USING ENV VARIABLES FOR SIMPLICITY)
void SupabaseClient::setSupabaseConfig(const QString &projectUrl, const QString &anonKey) {
   qDebug() << "[SupabaseClient] Config set - URL:" << m_projectUrl;
}

void SupabaseClient::setAccessToken(const QString &token) {
    m_accessToken = token;
    qDebug() << "[SupabaseClient] Access token set";
}

QNetworkRequest SupabaseClient::createRequest(const QString &endpoint, bool needsAuth) {
    QUrl url(m_projectUrl + "/rest/v1/" + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("apikey", m_anonKey.toUtf8());
    request.setRawHeader("Prefer", "return=representation");

    if (needsAuth && !m_accessToken.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + m_accessToken).toUtf8());
    }
    return request;
}

QString SupabaseClient::extractErrorMessage(const QJsonObject &json) {
    if (json.contains("message")) {
        return json["message"].toString();
    }
    if (json.contains("error")) {
        QString error = json["error"].toString();
        if (json.contains("error_description")) {
            return error + ": " + json["error_description"].toString();
        }
        return error;
    }
    return QString();
}

// User Profile Operations
void SupabaseClient::fetchUserProfile(const QString &userId) {
    if (m_projectUrl.isEmpty() || m_anonKey.isEmpty()) {
        emit userProfileError("Supabase configuration not set");
        return;
    }

    QString endpoint = QString("users?id=eq.%1").arg(userId);
    QNetworkRequest request = createRequest(endpoint);

    QNetworkReply *reply = m_networkManager->get(request);
    reply->setProperty("operation", "fetchUserProfile");
    reply->setProperty("userId", userId);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleUserProfileResponse(reply);
        reply->deleteLater();
    });
}

//save user profile to supabase
void SupabaseClient::saveUserProfile(const QVariantMap &userData) {
    if (m_projectUrl.isEmpty() || m_anonKey.isEmpty()) {
        emit userProfileError("Supabase configuration not set");
        return;
    }

    QJsonObject json;
    if (userData.contains("id")) json["id"] = userData["id"].toString();

    if (userData.contains("email") && !userData["email"].toString().isEmpty()) {
        json["email"] = userData["email"].toString();
    }
    if (userData.contains("user_name") && !userData["user_name"].toString().isEmpty()) {
        json["user_name"] = userData["user_name"].toString();
    }
    if (userData.contains("first_name") && !userData["first_name"].toString().isEmpty()) {
        json["first_name"] = userData["first_name"].toString();
    }
    if (userData.contains("last_name") && !userData["last_name"].toString().isEmpty()) {
        json["last_name"] = userData["last_name"].toString();
    }
    // Handle monthly_budget - convert to number or omit if empty
    if (userData.contains("monthly_budget")) {
        QString budgetStr = userData["monthly_budget"].toString();
        if (!budgetStr.isEmpty()) {
            bool ok;
            double budget = budgetStr.toDouble(&ok);
            if (ok) {
                json["monthly_budget"] = budget;
            }
            // If conversion fails, omit the field and use default 0.0
        }
    }
    if (userData.contains("profile_pic") && !userData["profile_pic"].toString().isEmpty()) {
        json["profile_pic"] = userData["profile_pic"].toString();
    }

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkRequest request = createRequest("users");
    QNetworkReply *reply = m_networkManager->post(request, data);
    reply->setProperty("operation", "saveUserProfile");
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleUserProfileResponse(reply);
        reply->deleteLater();
    });
}

//update user profile in supabase
void SupabaseClient::updateUserProfile(const QString &userId, const QVariantMap &userData) {
    if (m_projectUrl.isEmpty() || m_anonKey.isEmpty()) {
        emit userProfileError("Supabase configuration not set");
        return;
    }

    QJsonObject json;
    if (userData.contains("user_name") && !userData["user_name"].toString().isEmpty()) {
        json["user_name"] = userData["user_name"].toString();
    }
    if (userData.contains("first_name") && !userData["first_name"].toString().isEmpty()) {
        json["first_name"] = userData["first_name"].toString();
    }
    if (userData.contains("last_name") && !userData["last_name"].toString().isEmpty()) {
        json["last_name"] = userData["last_name"].toString();
    }
    if (userData.contains("email")) json["email"] = userData["email"].toString();
    // Handle monthly_budget - convert to number or omit if empty
    if (userData.contains("monthly_budget")) {
        QString budgetStr = userData["monthly_budget"].toString();
        if (!budgetStr.isEmpty()) {
            bool ok;
            double budget = budgetStr.toDouble(&ok);
            if (ok) {
                json["monthly_budget"] = budget;
            }
            // If conversion fails, omit the field and use default 0.0
        }
    }
    if (userData.contains("profile_pic") && !userData["profile_pic"].toString().isEmpty()) {
        json["profile_pic"] = userData["profile_pic"].toString();
    }

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QString endpoint = QString("users?id=eq.%1").arg(userId);
    QNetworkRequest request = createRequest(endpoint);
    QNetworkReply *reply = m_networkManager->sendCustomRequest(request, "PATCH", data);
    reply->setProperty("operation", "updateUserProfile");
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleUserProfileResponse(reply);
        reply->deleteLater();
    });
}

// fetch calendar entries from supabase
void SupabaseClient::fetchCalendarEntries(const QString &userId) {
    if (m_projectUrl.isEmpty() || m_anonKey.isEmpty()) {
        emit calendarError("Supabase configuration not set");
        return;
    }

    // Endpoint for fetching calendar entries sorted by start day
    QString endpoint = QString("calendar_entries?user_id=eq.%1&order=start_day.asc").arg(userId);
    QNetworkRequest request = createRequest(endpoint);

    QNetworkReply *reply = m_networkManager->get(request);
    reply->setProperty("operation", "fetchCalendarEntries");
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleCalendarResponse(reply, "fetchCalendarEntries");
        reply->deleteLater();
    });
}

// create calendar entry in supabase
void SupabaseClient::createCalendarEntry(const QString &userId, const QVariantMap &entryData) {
    if (m_projectUrl.isEmpty() || m_anonKey.isEmpty()) {
        emit calendarError("Supabase configuration not set");
        return;
    }

    QJsonObject json;
    json["user_id"] = userId;
    json["name"] = entryData["name"].toString();
    if (entryData.contains("description") && !entryData["description"].toString().isEmpty()) {
        json["description"] = entryData["description"].toString();
    }
    json["amount"] = entryData["amount"].toDouble();
    json["start_day"] = entryData["start_day"].toInt();
    // Only include end_day if it's valid (1-365), otherwise omit
    if (entryData.contains("end_day")) {
        int endDay = entryData["end_day"].toInt();
        if (endDay >= 1 && endDay <= 365) {
            json["end_day"] = endDay;
        }
        // If invalid, omit the field
    }
    if (entryData.contains("priority")) json["priority"] = entryData["priority"].toInt();
    if (entryData.contains("frequency")) json["frequency"] = entryData["frequency"].toString();
    if (entryData.contains("num_payees")) json["num_payees"] = entryData["num_payees"].toInt();

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkRequest request = createRequest("calendar_entries");
    QNetworkReply *reply = m_networkManager->post(request, data);
    reply->setProperty("operation", "createCalendarEntry");
    if (entryData.contains("_local_expense_id")) {
        reply->setProperty("localExpenseId", entryData["_local_expense_id"].toInt());
    }
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleCalendarResponse(reply, "createCalendarEntry");
        reply->deleteLater();
    });
}

// update calendar entry in supabase
void SupabaseClient::updateCalendarEntry(int entryId, const QVariantMap &entryData) {
    if (m_projectUrl.isEmpty() || m_anonKey.isEmpty()) {
        emit calendarError("Supabase configuration not set");
        return;
    }

    QJsonObject json;
    if (entryData.contains("name")) json["name"] = entryData["name"].toString();
    if (entryData.contains("description")) json["description"] = entryData["description"].toString();
    if (entryData.contains("amount")) json["amount"] = entryData["amount"].toDouble();
    if (entryData.contains("start_day")) json["start_day"] = entryData["start_day"].toInt();
    if (entryData.contains("end_day")) json["end_day"] = entryData["end_day"].toInt();
    if (entryData.contains("priority")) json["priority"] = entryData["priority"].toInt();
    if (entryData.contains("frequency")) json["frequency"] = entryData["frequency"].toString();
    if (entryData.contains("num_payees")) json["num_payees"] = entryData["num_payees"].toInt();

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QString endpoint = QString("calendar_entries?id=eq.%1").arg(entryId);
    QNetworkRequest request = createRequest(endpoint);
    QNetworkReply *reply = m_networkManager->sendCustomRequest(request, "PATCH", data);
    reply->setProperty("operation", "updateCalendarEntry");
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleCalendarResponse(reply, "updateCalendarEntry");
        reply->deleteLater();
    });
}

// delete calendar entry from supabase
void SupabaseClient::deleteCalendarEntry(int entryId) {
    if (m_projectUrl.isEmpty() || m_anonKey.isEmpty()) {
        emit calendarError("Supabase configuration not set");
        return;
    }

    QString endpoint = QString("calendar_entries?id=eq.%1").arg(entryId);
    QNetworkRequest request = createRequest(endpoint);
    QNetworkReply *reply = m_networkManager->deleteResource(request);
    reply->setProperty("operation", "deleteCalendarEntry");
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleCalendarResponse(reply, "deleteCalendarEntry");
        reply->deleteLater();
    });
}

// Helper to gather user profile from Supabase
void SupabaseClient::handleUserProfileResponse(QNetworkReply *reply) {
    QString operation = reply->property("operation").toString();

    if (reply->error() != QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(responseData);
        QJsonObject json = doc.object();
        QString errorMsg = extractErrorMessage(json);
        if (errorMsg.isEmpty()) errorMsg = reply->errorString();

        qDebug() << "[SupabaseClient] User profile error:" << errorMsg;
        emit userProfileError(errorMsg);
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);

    if (operation == "fetchUserProfile") {
        QJsonArray array = doc.array();
        if (array.size() > 0) {
            QJsonObject userObj = array[0].toObject();
            QVariantMap userData;
            userData["id"] = userObj["id"].toString();
            if (userObj.contains("user_name")) userData["user_name"] = userObj["user_name"].toString();
            if (userObj.contains("first_name")) userData["first_name"] = userObj["first_name"].toString();
            if (userObj.contains("last_name")) userData["last_name"] = userObj["last_name"].toString();
            if (userObj.contains("email")) userData["email"] = userObj["email"].toString();
            // Handle monthly_budget - it's numeric in database, preserve as numeric for proper conversion
            if (userObj.contains("monthly_budget")) {
                QJsonValue budgetValue = userObj["monthly_budget"];
                if (!budgetValue.isNull()) {
                    // Keep as numeric value - User class will convert to string properly
                    // This preserves the numeric type (double/int) instead of converting to string
                    userData["monthly_budget"] = budgetValue.toVariant();
                }
            }
            if (userObj.contains("profile_pic")) userData["profile_pic"] = userObj["profile_pic"].toString();

            emit userProfileFetched(userData);
        } else {
            // User profile doesn't exist yet - emit empty map so User class can create it
            emit userProfileFetched(QVariantMap());
        }
    } else if (operation == "saveUserProfile" || operation == "updateUserProfile") {
        emit userProfileSaved();
    }
}

// Helper to gather calendar entries from Supabase
void SupabaseClient::handleCalendarResponse(QNetworkReply *reply, const QString &operation) {
    if (reply->error() != QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(responseData);
        QJsonObject json = doc.object();
        QString errorMsg = extractErrorMessage(json);
        if (errorMsg.isEmpty()) errorMsg = reply->errorString();

        qDebug() << "[SupabaseClient] Calendar error:" << errorMsg;
        emit calendarError(errorMsg);
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);

    if (operation == "fetchCalendarEntries") {
        QJsonArray array = doc.array();
        // Emit signal with array of calendar entries
        emit calendarEntriesFetched(array);
    } else if (operation == "createCalendarEntry") {
        // Handle response for creating a new calendar entry
        QJsonArray array = doc.array();
        if (array.size() > 0) {
            int entryId = array[0].toObject()["id"].toInt();
            int localExpenseId = reply->property("localExpenseId").toInt();
            // Emit signal with both IDs so CalendarBridge can map them
            emit calendarEntryCreated(entryId, localExpenseId);
        }
    } else if (operation == "updateCalendarEntry") {
        // Handle response for updating a calendar entry
        emit calendarEntryUpdated();
    } else if (operation == "deleteCalendarEntry") {
        // Handle response for deleting a calendar entry
        emit calendarEntryDeleted();
    }
}

