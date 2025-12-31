#include "user.h"
#include "calendar_bridge.h"
#include "supabaseclient.h"
#include <QVariantMap>
#include <QDebug>

User::User(QObject *parent)
    : QObject(parent),
    m_calendar(new CalendarBridge(this)), // Initialize CalendarBridge owned by User
    m_supabaseClient(nullptr)
{}

// --- Getters ---
int User::userID() const { return m_userID; }
QString User::supabaseUserId() const { return m_supabaseUserId; }
QString User::userName() const { return m_userName; }
QString User::firstName() const { return m_firstName; }
QString User::lastName() const { return m_lastName; }
QString User::email() const { return m_email; }
QString User::password() const { return m_password; }
QString User::monthlyBudget() const { return m_monthlyBudget; }
QString User::profilePic() const { return m_profilePic; }
CalendarBridge* User::calendar() const { return m_calendar; } // getter for calendar

// --- Setters ---
void User::setUserID(int id) {
    if (m_userID != id) {
        m_userID = id;
        emit userIDChanged();
    }
}

void User::setSupabaseUserId(const QString &id) {
    if (m_supabaseUserId != id) {
        m_supabaseUserId = id;
        emit supabaseUserIdChanged();
    }
}

void User::setUserName(const QString &userName) {
    if (m_userName != userName) {
        m_userName = userName;
        emit userNameChanged();
    }
}

void User::setFirstName(const QString &first) {
    if (m_firstName != first) {
        m_firstName = first;
        emit firstNameChanged();
    }
}

void User::setLastName(const QString &last) {
    if (m_lastName != last) {
        m_lastName = last;
        emit lastNameChanged();
    }
}

void User::setEmail(const QString &mail) {
    if (m_email != mail) {
        m_email = mail;
        emit emailChanged();
    }
}

void User::setPassword(const QString &password) {
    if (m_password != password) {
        m_password = password;
        emit passwordChanged();
    }
}

void User::setMonthlyBudget(const QString &monthlyBudget) {
    if (m_monthlyBudget != monthlyBudget) {
        m_monthlyBudget = monthlyBudget;
        emit monthlyBudgetChanged();
    }
}

void User::setProfilePic(const QString &image) {
    if (m_profilePic != image) {
        m_profilePic = image;
        emit profilePicChanged();
    }
}

// --- Supabase sync methods ---
void User::setSupabaseClient(SupabaseClient *client) {
    if (m_supabaseClient != client) {
        if (m_supabaseClient) {
            disconnect(m_supabaseClient, nullptr, this, nullptr);
        }
        m_supabaseClient = client;
        if (m_supabaseClient) {
            connect(m_supabaseClient, &SupabaseClient::userProfileFetched,
                    this, &User::onUserProfileFetched);
            connect(m_supabaseClient, &SupabaseClient::userProfileSaved,
                    this, &User::onUserProfileSaved);
            connect(m_supabaseClient, &SupabaseClient::userProfileError,
                    this, &User::onUserProfileError);
        }
    }
}

void User::loadFromSupabase(const QString &userId) {
    if (!m_supabaseClient) {
        qDebug() << "[User] SupabaseClient not set, cannot load user data";
        emit userDataError("SupabaseClient not configured");
        return;
    }

    setSupabaseUserId(userId);
    m_supabaseClient->fetchUserProfile(userId);
}

void User::saveToSupabase() {
    if (!m_supabaseClient) {
        qDebug() << "[User] SupabaseClient not set, cannot save user data";
        emit userDataError("SupabaseClient not configured");
        return;
    }

    if (m_supabaseUserId.isEmpty()) {
        qDebug() << "[User] No Supabase user ID, cannot save";
        emit userDataError("User not authenticated");
        return;
    }

    QVariantMap userData;
    userData["id"] = m_supabaseUserId;
    userData["user_name"] = m_userName;
    userData["first_name"] = m_firstName;
    userData["last_name"] = m_lastName;
    userData["email"] = m_email;
    userData["monthly_budget"] = m_monthlyBudget;
    userData["profile_pic"] = m_profilePic;

    // Try to create new profile (for first-time users)
    m_supabaseClient->saveUserProfile(userData);
}

void User::updateInSupabase() {
    if (!m_supabaseClient) {
        qDebug() << "[User] SupabaseClient not set, cannot update user data";
        emit userDataError("SupabaseClient not configured");
        return;
    }

    if (m_supabaseUserId.isEmpty()) {
        qDebug() << "[User] No Supabase user ID, cannot update";
        emit userDataError("User not authenticated");
        return;
    }

    QVariantMap userData;
    userData["user_name"] = m_userName;
    userData["first_name"] = m_firstName;
    userData["last_name"] = m_lastName;
    userData["email"] = m_email;
    userData["monthly_budget"] = m_monthlyBudget;
    userData["profile_pic"] = m_profilePic;

    // Use update method (PATCH) for existing users
    m_supabaseClient->updateUserProfile(m_supabaseUserId, userData);
}

void User::onUserProfileFetched(const QVariantMap &userData) {
    // Check if userData is empty (user profile doesn't exist yet)
    if (userData.isEmpty() || !userData.contains("id")) {
        qDebug() << "[User] User profile not found, creating new profile";
        // Create a new user profile with current data
        saveToSupabase();
        return;
    }

    if (userData.contains("user_name")) setUserName(userData["user_name"].toString());
    if (userData.contains("first_name")) setFirstName(userData["first_name"].toString());
    if (userData.contains("last_name")) setLastName(userData["last_name"].toString());
    if (userData.contains("email")) setEmail(userData["email"].toString());
    if (userData.contains("monthly_budget")) {
        // Convert numeric monthly_budget to string, removing decimal if it's a whole number
        QVariant budgetVar = userData["monthly_budget"];
        QString budgetStr;
        if (budgetVar.type() == QVariant::Double || budgetVar.type() == QVariant::Int) {
            double budget = budgetVar.toDouble();
            if (budget == static_cast<int>(budget)) {
                budgetStr = QString::number(static_cast<int>(budget));
            } else {
                budgetStr = QString::number(budget, 'f', 2);
            }
        } else {
            budgetStr = budgetVar.toString();
        }
        setMonthlyBudget(budgetStr);
    }
    if (userData.contains("profile_pic")) setProfilePic(userData["profile_pic"].toString());

    qDebug() << "[User] User profile loaded from Supabase";
    emit userDataLoaded();
}

void User::onUserProfileSaved() {
    qDebug() << "[User] User profile saved to Supabase";
    emit userDataSaved();
    // Don't reload immediately - the local values are already correct
    // Reloading would cause a flicker and might overwrite with stale data
    // The data will be loaded fresh on next login
}

void User::onUserProfileError(const QString &error) {
    qDebug() << "[User] Error with Supabase:" << error;
    emit userDataError(error);
}
