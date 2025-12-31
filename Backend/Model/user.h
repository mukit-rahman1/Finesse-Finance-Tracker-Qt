#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include "calendar_bridge.h"  // or "calendar.h" if using Calendar directly

class SupabaseClient;

class User : public QObject {
    Q_OBJECT

    // User properties
    Q_PROPERTY(int userID READ userID WRITE setUserID NOTIFY userIDChanged)
    Q_PROPERTY(QString supabaseUserId READ supabaseUserId WRITE setSupabaseUserId NOTIFY supabaseUserIdChanged)
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName NOTIFY firstNameChanged)
    Q_PROPERTY(QString lastName READ lastName WRITE setLastName NOTIFY lastNameChanged)
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString monthlyBudget READ monthlyBudget WRITE setMonthlyBudget NOTIFY monthlyBudgetChanged)
    Q_PROPERTY(QString profilePic READ profilePic WRITE setProfilePic NOTIFY profilePicChanged)

    // Expose CalendarBridge to QML
    Q_PROPERTY(CalendarBridge* calendar READ calendar CONSTANT)

public:
    explicit User(QObject *parent = nullptr);

    // --- Getters ---
    int userID() const;
    QString supabaseUserId() const;
    QString userName() const;
    QString firstName() const;
    QString lastName() const;
    QString email() const;
    QString password() const;
    QString monthlyBudget() const;
    QString profilePic() const;
    CalendarBridge* calendar() const;  // getter for user's calendar

    // --- Setters ---
    void setUserID(int id);
    void setSupabaseUserId(const QString &id);
    void setUserName(const QString &userName);
    void setFirstName(const QString &first);
    void setLastName(const QString &last);
    void setEmail(const QString &mail);
    void setPassword(const QString &password);
    void setMonthlyBudget(const QString &monthlyBudget);
    void setProfilePic(const QString &image);

    // --- Supabase sync methods ---
    void setSupabaseClient(SupabaseClient *client);
    void loadFromSupabase(const QString &userId);
    void saveToSupabase();
    Q_INVOKABLE void updateInSupabase();

signals:
    void userIDChanged();
    void supabaseUserIdChanged();
    void userNameChanged();
    void firstNameChanged();
    void lastNameChanged();
    void emailChanged();
    void passwordChanged();
    void monthlyBudgetChanged();
    void profilePicChanged();
    void userDataLoaded();
    void userDataSaved();
    void userDataError(const QString &error);

private slots:
    void onUserProfileFetched(const QVariantMap &userData);
    void onUserProfileSaved();
    void onUserProfileError(const QString &error);

private:
    int m_userID = 0;
    QString m_supabaseUserId;
    QString m_userName;
    QString m_firstName;
    QString m_lastName;
    QString m_email;
    QString m_password;
    QString m_monthlyBudget;
    QString m_profilePic;

    CalendarBridge* m_calendar;  // each user has their own calendar
    SupabaseClient* m_supabaseClient;
};

#endif // USER_H
