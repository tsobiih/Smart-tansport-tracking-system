#include "json_helper.h"

#include <QFile>
#include <QSaveFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

extern "C" {

  #include "passenger_manager.h"
  #include "driver_manager.h"
  #include "queue_manager.h"

}

/*
  json helper (Qt). التعليقات: هذا ملف C++ ويغلف دواله ب extern "C" حتى يمكن استدعاؤها من الـ C.
*/

extern "C" {

void save_passengers_to_json()
{
    QJsonArray jsonArray;

    PassengerNode* current_p = all_passengers_head;

    while(current_p != NULL)
    {
        QJsonObject obj;
        obj["id"] = current_p->p_data.id;
        obj["fullName"] = current_p->p_data.name;
        obj["email"] = current_p->p_data.email;
        obj["password"] = current_p->p_data.password;
        obj["mobile"] = current_p->p_data.mobile;

        jsonArray.append(obj);
        current_p = current_p->next;
    }

    QSaveFile pFile("passengers.json");
    if(pFile.open(QIODevice::WriteOnly))
    {
        QJsonDocument doc(jsonArray);
        pFile.write(doc.toJson());
        if (!pFile.commit()) {
            qDebug() << "Failed to commit passengers.json:" << pFile.errorString();
        } else {
            qDebug() << "Passengers saved successfully.";
        }
    } else {
        qDebug() << "Error opening passengers file for writing:" << pFile.errorString();
    }
}

void load_passengers_from_json()
{
    QFile dFile("passengers.json");
    if (!dFile.open(QIODevice::ReadOnly)) {
        qDebug() << "No passengers file found.";
        return;
    }

    QByteArray data = dFile.readAll();
    dFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        qDebug() << "Invalid passengers.json format.";
        return;
    }

    QJsonArray jsonArray = doc.array();

    for(const QJsonValue &value : jsonArray)
    {
        if (!value.isObject()) continue;
        QJsonObject obj = value.toObject();
        Passenger p;
        memset(&p, 0, sizeof(p));

        QByteArray tmp;

        tmp = obj.value("fullName").toString().toUtf8();
        strncpy(p.name, tmp.constData(), sizeof(p.name) - 1);
        p.name[sizeof(p.name) - 1] = '\0';

        tmp = obj.value("mobile").toString().toUtf8();
        strncpy(p.mobile, tmp.constData(), sizeof(p.mobile) - 1);
        p.mobile[sizeof(p.mobile) - 1] = '\0';

        tmp = obj.value("password").toString().toUtf8();
        strncpy(p.password, tmp.constData(), sizeof(p.password) - 1);
        p.password[sizeof(p.password) - 1] = '\0';

        tmp = obj.value("email").toString().toUtf8();
        strncpy(p.email, tmp.constData(), sizeof(p.email) - 1);
        p.email[sizeof(p.email) - 1] = '\0';

        p.id = obj.value("id").toInt();

        add_passenger_to_list(p);
    }
    qDebug() << "Passengers loaded successfully: " << jsonArray.size();
}

void save_drivers_to_json()
{
    QJsonArray jsonArray;

    DriverNode* current = all_drivers_head;

    while(current != NULL)
    {
        QJsonObject obj;
        obj["name"] = current->data.name;
        obj["mobile"] = current->data.mobile;
        obj["password"] = current->data.password;
        obj["bus_id"] = current->data.bus_id;
        obj["dest"] = (int)current->data.dest; // casting enum to int
        obj["status"] = (int)current->data.status;

        jsonArray.append(obj);
        current = current->next;
    }

    QSaveFile dFile("drivers.json");
    if(dFile.open(QIODevice::WriteOnly))
    {
        QJsonDocument doc(jsonArray);
        dFile.write(doc.toJson());
        if (!dFile.commit()) {
            qDebug() << "Failed to commit drivers.json:" << dFile.errorString();
        } else {
            qDebug() << "Drivers saved successfully.";
        }
    } else {
        qDebug() << "Error opening drivers file for writing:" << dFile.errorString();
    }
}

void load_drivers_from_json()
{
    QFile dFile("drivers.json");
    if (!dFile.open(QIODevice::ReadOnly)) {
        qDebug() << "No drivers file found.";
        return;
    }

    QByteArray data = dFile.readAll();
    dFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        qDebug() << "Invalid drivers.json format.";
        return;
    }

    QJsonArray jsonArray = doc.array();

    for(const QJsonValue &value : jsonArray)
    {
        if (!value.isObject()) continue;
        QJsonObject obj = value.toObject();
        Driver d;
        memset(&d, 0, sizeof(d));

        QByteArray tmp;

        tmp = obj.value("name").toString().toUtf8();
        strncpy(d.name, tmp.constData(), sizeof(d.name) - 1);
        d.name[sizeof(d.name) - 1] = '\0';

        tmp = obj.value("mobile").toString().toUtf8();
        strncpy(d.mobile, tmp.constData(), sizeof(d.mobile) - 1);
        d.mobile[sizeof(d.mobile) - 1] = '\0';

        tmp = obj.value("password").toString().toUtf8();
        strncpy(d.password, tmp.constData(), sizeof(d.password) - 1);
        d.password[sizeof(d.password) - 1] = '\0';

        tmp = obj.value("bus_id").toString().toUtf8();
        strncpy(d.bus_id, tmp.constData(), sizeof(d.bus_id) - 1);
        d.bus_id[sizeof(d.bus_id) - 1] = '\0';

        int destInt = obj.value("dest").toInt();
        if (destInt >= 0 && destInt < DEST_COUNT) d.dest = (Destination)destInt;
        else d.dest = CAIRO_SHOUBRA; // fallback

        int statusInt = obj.value("status").toInt();
        if (statusInt >= STATUS_NOT_ACTIVE && statusInt <= STATUS_ON_TRIP) d.status = (DriverStatus)statusInt;
        else d.status = STATUS_NOT_ACTIVE;

        add_driver_to_list(d);
    }
    qDebug() << "Drivers loaded successfully: " << jsonArray.size();
}

void save_queues_to_json()
{
    QJsonObject rootObj;

    QJsonArray queuesArray;

    for (int i = 0; i < DEST_COUNT; ++i) {

        QJsonObject obj;
        obj["dest_id"] = i;
        obj["dest_name"] = dest_id_to_dist_name(i);

        QJsonArray driversArr;

        DriverNode* current_d = station_queues[i].head;
        while(current_d != NULL)
        {
            driversArr.append(current_d->data.mobile);
            current_d = current_d->next;
        }
        obj["waiting_drivers"] = driversArr;

        queuesArray.append(obj);
    }

    rootObj["queues"] = queuesArray;

    rootObj["active_trips"] = QJsonArray();

    QSaveFile tFile("trips_state.json");
    if(tFile.open(QIODevice::WriteOnly))
    {
        QJsonDocument doc(rootObj);
        tFile.write(doc.toJson());
        if (!tFile.commit()) {
            qDebug() << "Failed to commit trips_state.json:" << tFile.errorString();
        } else {
            qDebug() << "Queues saved successfully.";
        }
    } else {
        qDebug() << "Error opening trips_state file for writing:" << tFile.errorString();
    }
}

void load_queues_from_json()
{
    QFile tFile("trips_state.json");
    if (!tFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Warning: trips_state.json not found or cannot be opened.";
        return;
    }

    QByteArray data = tFile.readAll();
    tFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Error: Failed to parse trips_state.json";
        return;
    }

    QJsonObject rootObj = doc.object();

    /* Empty queues once */
    init_queues();

    if (rootObj.contains("queues") && rootObj["queues"].isArray()) {
        QJsonArray queuesArray = rootObj["queues"].toArray();
        qDebug() << "Load Queues: " << queuesArray.size();

        for (const QJsonValue &qVal : queuesArray) {
            if (!qVal.isObject()) continue;
            QJsonObject queueObj = qVal.toObject();

            int destID = queueObj.value("dest_id").toInt(-1);
            if (destID < 0 || destID >= DEST_COUNT) {
                qDebug() << "Skipping queue with invalid dest_id:" << destID;
                continue;
            }

            if (queueObj.contains("waiting_drivers") && queueObj["waiting_drivers"].isArray()) {
                QJsonArray driversArr = queueObj["waiting_drivers"].toArray();

                for (const QJsonValue &dVal : driversArr) {
                    QString mobile = dVal.toString();

                    DriverNode* current_driver = all_drivers_head;
                    while(current_driver != NULL)
                    {
                        if (QString::fromUtf8(current_driver->data.mobile) == mobile)
                        {
                            current_driver->data.dest = (Destination)destID;
                            enqueue_driver(current_driver->data);
                            break;
                        }
                        current_driver = current_driver->next;
                    }
                }
            }
        }
    }

    if (rootObj.contains("active_trips") && rootObj["active_trips"].isArray()) {
        QJsonArray tripsArray = rootObj["active_trips"].toArray();
        qDebug() << " Loading Active Trips: " << tripsArray.size();
    }

    qDebug() << "State Loaded Successfully";
}

} // extern "C"
