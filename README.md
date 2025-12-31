# Finesse — Finance Tracker (Qt/QML + Supabase)

Finesse is a lightweight personal finance tracker built with Qt (C++) + QML in Qt Creator.  
It tracks expenses, computes monthly totals/remaining budget, and visualizes spending with a pie chart.  
User authentication and cloud-backed data storage are handled with Supabase.

## Project Goal
- Practice OOP within C++
- Get familiar with QtCreator
- Learn to connect Supabase with C++.

## Tech Stack
- **Qt 6 / Qt Creator**
- **C++ + QML**
- **Supabase** (Auth + Database via REST)
- Qt modules: `QtNetwork`, `QtQml`, `QtQuick`

---

## Features
- Email/password login (**Supabase Auth**)
- Monthly budget + remaining budget calculation
- Expense calendar (add/remove payments)
- Dashboard totals + “recent” expense updates
- Pie chart visualization

---

## Prerequisites
- **Qt 6.x** installed (Qt Online Installer)
- **Qt Creator**
- A **Supabase project** (Project URL + anon key)

> Note: The Supabase **anon** key is designed to be used in client apps. Protect data using Supabase **Row Level Security (RLS)**. Never ship a `service_role` key in a client app.

---

## Installation & Setup
### Step 1  
Create a Supabase project

In Supabase:

Create a new project
Enable Email/Password auth

Copy:

Project URL
Anon key

### Step 2  
Configure Supabase credentials using QtCreator environment variables
This app reads credentials from environment variables:

SUPABASE_URL  
SUPABASE_ANON_KEY

Click Projects (wrench icon on left)

Select your Desktop Kit
Go to Run

Under Environment, add:

SUPABASE_URL=<Your Supabase URL>

SUPABASE_ANON_KEY=<Your Supabase Anon Key>

### Step 3 
Click on build and build all Projects  
Hit run

**Notes:** 
- Password must be at least six characters long. Email does not have to be real if "confirm email" is off on Supabase authentication.
- Same page used for registeration and log in for simplicity

### Authors
Made in ELEC376 Software Development Methology group project

## Pictures
**More Pictures of Pages can be found in Pics folder**. 
Database Breakdown:
![tables](https://github.com/mukit-rahman1/Finesse-Finance-Tracker-Qt/blob/main/Pics/tables.png?raw=true)

Home Page:  
![home](https://github.com/mukit-rahman1/Finesse-Finance-Tracker-Qt/blob/main/Pics/home.png?raw=true)

Profile Page:  
![profile](https://github.com/mukit-rahman1/Finesse-Finance-Tracker-Qt/blob/main/Pics/profile.png?raw=true)

Summary Page:  
![Summary](https://github.com/mukit-rahman1/Finesse-Finance-Tracker-Qt/blob/main/Pics/chart.png?raw=true)
