#ifndef SUPABASEAUTH_H
#define SUPABASEAUTH_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

class SupabaseAuth : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY authenticationChanged)
    Q_PROPERTY(QString userId READ userId NOTIFY authenticationChanged)
    Q_PROPERTY(QString userEmail READ userEmail NOTIFY authenticationChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorOccurred)

public:
    explicit SupabaseAuth(QObject *parent = nullptr);

    // Set Supabase project URL and anon key
    Q_INVOKABLE void setSupabaseConfig(const QString &projectUrl, const QString &anonKey);

    // Unified login/signup - tries login first, then signup if user doesn't exist
    Q_INVOKABLE void signInOrSignUp(const QString &email, const QString &password);

    // Sign out
    Q_INVOKABLE void signOut();

    // Getters
    bool isAuthenticated() const { return m_isAuthenticated; }
    QString userId() const { return m_userId; }
    QString userEmail() const { return m_userEmail; }
    QString errorMessage() const { return m_errorMessage; }
    QString accessToken() const { return m_accessToken; }

signals:
    void authenticationChanged();
    void authenticationSucceeded(const QString &userId, const QString &email);
    void authenticationFailed(const QString &errorMessage);
    void errorOccurred();

private:
    void handleSignInResponse(QNetworkReply *reply);
    void handleSignUpResponse(QNetworkReply *reply);
    void attemptSignUp(const QString &email, const QString &password);
    QString extractErrorMessage(const QJsonObject &json);

    QNetworkAccessManager *m_networkManager;
    QString m_projectUrl;
    QString m_anonKey;
    QString m_accessToken;
    QString m_refreshToken;
    bool m_isAuthenticated;
    QString m_userId;
    QString m_userEmail;
    QString m_errorMessage;
    QString m_pendingEmail;
    QString m_pendingPassword;
    bool m_isSigningUp; // Flag to prevent infinite loop
    bool m_justSignedUp; // Flag to prevent retrying signup after failed sign-in
};

#endif // SUPABASEAUTH_H

