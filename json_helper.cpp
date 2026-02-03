#include "json_helper.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QtTypes>

extern "C" {

  #include "passenger_manager.h"
  #include "driver_manager.h"
  #include "queue_manager.h"

}

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
        obj["lastTrip"] = current_p->p_data.lastTrip;

        jsonArray.append(obj);
        current_p = current_p->next;
    }

    QFile pFile("passengers.json");
    if(pFile.open(QIODevice::WriteOnly))
    {
        QJsonDocument doc(jsonArray);
        pFile.write(doc.toJson());
        pFile.close();
        qDebug() << "Passengers saved successfully.";
    } else {
        qDebug() << "Error saving passengers file!";
    }
}

qint32 load_passengers_from_json()
{
    qint32 id;
    QFile dFile("passengers.json");
    if (!dFile.open(QIODevice::ReadOnly)) {
        qDebug() << "No passengers file found.";
        return 0;
    }

    QByteArray data = dFile.readAll();
    dFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray jsonArray = doc.array();

    // To embty the current list (optional)
    init_passenger_list();

    for (const QJsonValue &value : jsonArray)
    {
        QJsonObject obj = value.toObject();
        Passenger p;

        strcpy(p.name, obj["fullName"].toString().toUtf8().constData());
        strcpy(p.mobile, obj["mobile"].toString().toUtf8().constData());
        strcpy(p.password, obj["password"].toString().toUtf8().constData());
        strcpy(p.email, obj["email"].toString().toUtf8().constData());
        strcpy(p.lastTrip, obj["lastTrip"].toString().toUtf8().constData());

        p.id = obj["id"].toInt();
        id = obj["id"].toInt();
        add_passenger_to_list(p);
    }
    qDebug() << "Drivers loaded successfully: " << jsonArray.size();
    return id;
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

    QFile dFile("drivers.json");
    if(dFile.open(QIODevice::WriteOnly))
    {
        QJsonDocument doc(jsonArray);
        dFile.write(doc.toJson());
        dFile.close();
        qDebug() << "Drivers saved successfully.";
    } else {
        qDebug() << "Error saving drivers file!";
    }
}

char* load_drivers_from_json()
{
    char temp_id[10];
    QFile dFile("drivers.json");
    if (!dFile.open(QIODevice::ReadOnly)) {
        qDebug() << "No drivers file found.";
        return NULL;
    }

    QByteArray data = dFile.readAll();
    dFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray jsonArray = doc.array();

    // To embty the current list (optional)
    init_drivers_list();

    for(const QJsonValue &value : jsonArray)
    {
        QJsonObject obj = value.toObject();
        Driver d;

        strcpy(d.name, obj["name"].toString().toUtf8().constData());
        strcpy(d.mobile, obj["mobile"].toString().toUtf8().constData());
        strcpy(d.password, obj["password"].toString().toUtf8().constData());
        strcpy(d.bus_id, obj["bus_id"].toString().toUtf8().constData());
        strcpy(temp_id, obj["bus_id"].toString().toUtf8().constData());

        d.dest = (Destination)obj["dest"].toInt();
        d.status = (DriverStatus)obj["status"].toInt();

        add_driver_to_list(d);
    }
    qDebug() << "Drivers loaded successfully: " << jsonArray.size();
    return temp_id;
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

    QFile tFile("trips_state.json");
    if(tFile.open(QIODevice::WriteOnly))
    {
        QJsonDocument doc(rootObj);
        tFile.write(doc.toJson());
        tFile.close();
        qDebug() << "Queues saved successfully.";
    } else {
        qDebug() << "Error saving queues file!";
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

    // check the file is not damaged
    if (doc.isNull()) {
        qDebug() << "Error: Failed to parse trips_state.json";
        return;
    }

    QJsonObject rootObj = doc.object();
    if (rootObj.contains("queues") && rootObj["queues"].isArray()) {
        QJsonArray queuesArray = rootObj["queues"].toArray();
        qDebug() << " Load Queues";
        // parse the array here

        for (const QJsonValue &qVal : queuesArray) {
            QJsonObject queueObj = qVal.toObject();

            int destID = queueObj["dest_id"].toInt();
            QString destName = queueObj["dest_name"].toString();

            // parse drivers' mobiles
            if (queueObj.contains("waiting_drivers") && queueObj["waiting_drivers"].isArray()) {
                QJsonArray driversArr = queueObj["waiting_drivers"].toArray();

                for (const QJsonValue &dVal : driversArr) {
                    QString mobile = dVal.toString();

                    // Empty the current queue (optional)
                    init_queues();

                    DriverNode* current_driver = all_drivers_head;
                    while(current_driver != NULL)
                    {
                        if(current_driver->data.mobile == mobile)
                        {
                            // Set the queue
                            enqueue_driver(current_driver->data);
                        }
                    }
                }
            }
        }
    }

    if (rootObj.contains("active_trips") && rootObj["active_trips"].isArray()) {
        QJsonArray tripsArray = rootObj["active_trips"].toArray();
        qDebug() << " Loading Active Trips ";
        // parse the array here
    }

    qDebug() << "State Loaded Successfully";
}
