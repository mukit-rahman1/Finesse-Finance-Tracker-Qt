#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDate>
#include <QDebug>
#include <QMap>

#include "calendar_bridge.h"
#include "dashboardbridge.h"
#include "paymentsmanager.h"
#include "user.h"
#include "pie.h"
#include "supabaseauth.h"
#include "supabaseclient.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // Supabase configuration (NOT USING ENV VARIABLES FOR SIMPLICITY)
    QString supabaseUrl = "https://desqwioiubqlcrtqnxqw.supabase.co";
    QString supabaseAnonKey =
        "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImRlc3F3aW9pdWJxbGNydHFueHF3Iiwicm9sZSI6ImFub24iLCJpYXQiOjE3NjMwNDk2NzcsImV4cCI6MjA3ODYyNTY3N30.mUhffcr1Lt2rn9IEaJ1m399lIWrSVIniLOGZhIpgMSY";

    // Initialize Supabase Auth
    auto *auth = new SupabaseAuth(&app);
    auth->setSupabaseConfig(supabaseUrl, supabaseAnonKey);
    engine.rootContext()->setContextProperty("auth", auth);

    // Initialize Supabase Client for database operations
    auto *supabaseClient = new SupabaseClient(&app);
    supabaseClient->setSupabaseConfig(supabaseUrl, supabaseAnonKey);
    engine.rootContext()->setContextProperty("supabaseClient", supabaseClient);

    // User object
    auto *user = new User(&app);
    // Set default values (will be overridden when loading from Supabase)
    user->setFirstName("Alice");
    user->setLastName("Smith");
    user->setPassword("12345");
    user->setProfilePic("../assests/JohnDoe.png");

    // Wire User and CalendarBridge to SupabaseClient
    user->setSupabaseClient(supabaseClient);
    // Use the SAME CalendarBridge instance everywhere (user->calendar())
    CalendarBridge *cal = user->calendar();
    cal->setSupabaseClient(supabaseClient);

    auto *dashboard = new DashboardBridge(&app);

    //get date for dashboard
    QString today = QDate::currentDate().toString("MMM d, yyyy");
    dashboard->setRecent(0.0, today);

    auto *payments  = new PaymentsManager(&app);

    // Connect auth success to load user data and calendar
    QObject::connect(auth, &SupabaseAuth::authenticationSucceeded,
                     [auth, supabaseClient, user](const QString &userId, const QString &email) {
                         qDebug() << "[main] Authentication succeeded, loading user data for:" << userId;

                         // Set email from auth response (before loading from Supabase)
                         user->setEmail(email);
                         user->setSupabaseUserId(userId);

                         // Set access token for database operations
                         supabaseClient->setAccessToken(auth->accessToken());

                         // Set user ID for calendar
                         user->calendar()->setUserId(userId);

                         // Load user profile from Supabase
                         user->loadFromSupabase(userId);

                         // Load calendar entries from Supabase
                         user->calendar()->loadFromSupabase();
                     });

    // Expose to QML
    engine.rootContext()->setContextProperty("user", user);
    engine.rootContext()->setContextProperty("Cal", cal);
    engine.rootContext()->setContextProperty("dashboard", dashboard);
    engine.rootContext()->setContextProperty("paymentsManager", payments);

    // Month → (startDay, endDay) mapping
    QMap<QString, QPair<int, int>> dateMap;
    dateMap["Jan"] = qMakePair(1, 31);
    dateMap["Feb"] = qMakePair(32, 59);
    dateMap["Mar"] = qMakePair(60, 90);
    dateMap["Apr"] = qMakePair(91, 120);
    dateMap["May"] = qMakePair(121, 151);
    dateMap["Jun"] = qMakePair(152, 181);
    dateMap["Jul"] = qMakePair(182, 212);
    dateMap["Aug"] = qMakePair(213, 243);
    dateMap["Sep"] = qMakePair(244, 273);
    dateMap["Oct"] = qMakePair(274, 304);
    dateMap["Nov"] = qMakePair(305, 334);
    dateMap["Dec"] = qMakePair(335, 365);

    // Pie chart bridge
    auto *pieBridge = new PieChartBridge(&app);
    pieBridge->setTotalFunds(user->monthlyBudget().toDouble()); // will be updated when budget changes
    engine.rootContext()->setContextProperty("pieBridge", pieBridge);

    int currentMonthIndex = QDate::currentDate().month() - 1;
    pieBridge->setMonthIndex(currentMonthIndex);
    // Initialize update to populate pie chart using the unified calendar
    pieBridge->updateFromCalendar(cal);

    // Function for recomputing the totals for the current month
    auto recomputeForCurrentMonth = [cal, dashboard, user, &dateMap]() {
        double total = 0.0;
        QString monthKey = QDate::currentDate().toString("MMM");

        for (int d = dateMap[monthKey].first; d <= dateMap[monthKey].second; ++d) {
            QVariantList list = cal->expensesForDay(d);
            for (const QVariant &v : list) {
                QVariantMap m = v.toMap();
                total += m.value("amount").toDouble();
            }
        }

        double budget    = user->monthlyBudget().toDouble();
        double remaining = budget - total;

        dashboard->setTotals(total, remaining);
    };

    // Initial computation
    recomputeForCurrentMonth();

    // Connect calendar changes to pie chart updates & totals
    QObject::connect(cal, &CalendarBridge::paymentAdded,
                     [cal, pieBridge, recomputeForCurrentMonth](double) {
                         pieBridge->updateFromCalendar(cal);
                         recomputeForCurrentMonth();
                     });
    QObject::connect(cal, &CalendarBridge::paymentRemoved,
                     [cal, pieBridge, recomputeForCurrentMonth](double) {
                         pieBridge->updateFromCalendar(cal);
                         recomputeForCurrentMonth();
                     });

    // Budget remaining should update as soon as monthly budget changes
    QObject::connect(user, &User::monthlyBudgetChanged,
                     &app,
                     [recomputeForCurrentMonth, pieBridge, user]() {
                         qDebug() << "[main] Monthly budget changed to" << user->monthlyBudget();

                         // Update pie chart total funds
                         if (pieBridge) {
                             pieBridge->setTotalFunds(user->monthlyBudget().toDouble());
                         }

                         // Recompute totals/remaining with the new budget
                         recomputeForCurrentMonth();
                     });

    // Dashboard "recent" & totals recompute on add/remove (per current month)
    QObject::connect(cal, &CalendarBridge::paymentAdded,
                     [cal, dashboard, user, dateMap](double amount) {
                         double total = 0.0;
                         QString recentDate = QDate::currentDate().toString("MMM d, yyyy");
                         QString monthKey   = QDate::currentDate().toString("MMM");

                         for (int d = dateMap[monthKey].first; d <= dateMap[monthKey].second; ++d) {
                             qDebug() << "d =" << d << " size =" << cal->expensesForDay(d).size();
                             QVariantList list = cal->expensesForDay(d);
                             for (const QVariant &v : list) {
                                 QVariantMap m = v.toMap();
                                 qDebug() << m["amount"];
                                 total += m["amount"].toDouble();
                             }
                         }

                         double budget    = user->monthlyBudget().toDouble();
                         double remaining = budget - total;
                         dashboard->setTotals(total, remaining);
                         dashboard->setRecent(amount, recentDate);
                         qDebug() << "[main] dashboard updated from CalendarBridge: total=" << total
                                  << " recent=" << amount;
                     });

    QObject::connect(cal, &CalendarBridge::paymentRemoved,
                     [cal, dashboard, user, dateMap](double removedAmount) {
                         double total = 0.0;
                         QString recentDate = QDate::currentDate().toString("MMM d, yyyy");
                         QString monthKey   = QDate::currentDate().toString("MMM");

                         for (int d = dateMap[monthKey].first; d <= dateMap[monthKey].second; ++d) {
                             QVariantList list = cal->expensesForDay(d);
                             for (const QVariant &v : list) {
                                 QVariantMap m = v.toMap();
                                 total += m["amount"].toDouble();
                             }
                         }

                         double budget    = user->monthlyBudget().toDouble();
                         double remaining = budget - total;
                         dashboard->setTotals(total, remaining);
                         dashboard->setRecent(0.0, recentDate); // after removal, treat "recent" as cleared

                         qDebug() << "[main] dashboard updated after removal: total=" << total
                                  << " removed=" << removedAmount;
                     });

    engine.load(QUrl(u"Main.qml"_qs));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
