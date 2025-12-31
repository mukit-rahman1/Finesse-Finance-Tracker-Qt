#ifndef SUPABASECLIENT_H
#define SUPABASECLIENT_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>

class SupabaseClient : public QObject {
    Q_OBJECT

public:
    explicit SupabaseClient(QObject *parent = nullptr);

    // Configuration
    void setSupabaseConfig(const QString &projectUrl, const QString &anonKey);
    void setAccessToken(const QString &token);

    // User profile operations
    Q_INVOKABLE void fetchUserProfile(const QString &userId);
    Q_INVOKABLE void saveUserProfile(const QVariantMap &userData);
    Q_INVOKABLE void updateUserProfile(const QString &userId, const QVariantMap &userData);

    // Calendar entries operations
    Q_INVOKABLE void fetchCalendarEntries(const QString &userId);
    Q_INVOKABLE void createCalendarEntry(const QString &userId, const QVariantMap &entryData);
    Q_INVOKABLE void updateCalendarEntry(int entryId, const QVariantMap &entryData);
    Q_INVOKABLE void deleteCalendarEntry(int entryId);

signals:
    void userProfileFetched(const QVariantMap &userData);
    void userProfileSaved();
    void userProfileError(const QString &error);

    void calendarEntriesFetched(const QJsonArray &entries);
    void calendarEntryCreated(int entryId, int localExpenseId = -1);
    void calendarEntryUpdated();
    void calendarEntryDeleted();
    void calendarError(const QString &error);

private:
    QNetworkRequest createRequest(const QString &endpoint, bool needsAuth = true);
    QString extractErrorMessage(const QJsonObject &json);
    void handleUserProfileResponse(QNetworkReply *reply);
    void handleCalendarResponse(QNetworkReply *reply, const QString &operation);

    QNetworkAccessManager *m_networkManager;
    QString m_projectUrl;
    QString m_anonKey;
    QString m_accessToken;
};

#endif // SUPABASECLIENT_H

