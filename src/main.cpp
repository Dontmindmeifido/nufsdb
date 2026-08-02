#include "interface/interface.h"
#include "disk_storage/manager.h"

int main() {
    Database::get_instance()->create_table("GUESTS", 
        std::vector<std::string> {"FIRST_NAME", "LAST_NAME", "PHONE", "TIER"}, 
        std::vector<std::string> {"VARCHAR", "VARCHAR", "NUMBER", "VARCHAR"});
    
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"James", "Holden", "5550192", "GOLD"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Amos", "Burton", "5559981", "PLATINUM"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Naomi", "Nagata", "5557762", "SILVER"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Alex", "Kamal", "5553344", "BRONZE"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Chrisjen", "Avasarala", "5550001", "PLATINUM"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Bobbie", "Draper", "5558877", "GOLD"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Josephus", "Miller", "5554433", "NONE"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Julie", "Mao", "5552211", "SILVER"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"James", "Holden", "5550192", "GOLD"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Amos", "Burton", "5559981", "PLATINUM"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Naomi", "Nagata", "5557762", "SILVER"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Alex", "Kamal", "5553344", "BRONZE"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Chrisjen", "Avasarala", "5550001", "PLATINUM"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Bobbie", "Draper", "5558877", "GOLD"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Josephus", "Miller", "5554433", "NONE"});
    Database::get_instance()->insert_row("GUESTS", std::vector<std::string> {"Julie", "Mao", "5552212", "SILVER"});

    Database::get_instance()->create_table("ROOMS", 
        std::vector<std::string> {"ROOM_NUM", "TYPE", "RATE", "STATUS"}, 
        std::vector<std::string> {"NUMBER", "VARCHAR", "NUMBER", "VARCHAR"});
        
    Database::get_instance()->insert_row("ROOMS", std::vector<std::string> {"101", "STANDARD", "150", "CLEAN"});
    Database::get_instance()->insert_row("ROOMS", std::vector<std::string> {"102", "STANDARD", "150", "OCCUPIED"});
    Database::get_instance()->insert_row("ROOMS", std::vector<std::string> {"103", "DOUBLE", "200", "CLEAN"});
    Database::get_instance()->insert_row("ROOMS", std::vector<std::string> {"201", "DELUXE", "350", "MAINTENANCE"});
    Database::get_instance()->insert_row("ROOMS", std::vector<std::string> {"202", "DELUXE", "350", "OCCUPIED"});
    Database::get_instance()->insert_row("ROOMS", std::vector<std::string> {"301", "SUITE", "800", "CLEAN"});
    Database::get_instance()->insert_row("ROOMS", std::vector<std::string> {"302", "SUITE", "800", "CLEAN"});
    Database::get_instance()->insert_row("ROOMS", std::vector<std::string> {"999", "PENTHOUSE", "2500", "OCCUPIED"});

    Database::get_instance()->create_table("RESERVATIONS", 
        std::vector<std::string> {"RES_ID", "ROOM_NUM", "LAST_NAME", "CHECK_IN", "NIGHTS"}, 
        std::vector<std::string> {"NUMBER", "NUMBER", "VARCHAR", "DATETIME", "NUMBER"});
        
    Database::get_instance()->insert_row("RESERVATIONS", std::vector<std::string> {"8001", "102", "Holden", "12-04-2026", "3"});
    Database::get_instance()->insert_row("RESERVATIONS", std::vector<std::string> {"8002", "202", "Avasarala", "14-04-2026", "5"});
    Database::get_instance()->insert_row("RESERVATIONS", std::vector<std::string> {"8003", "999", "Mao", "20-04-2026", "14"});
    Database::get_instance()->insert_row("RESERVATIONS", std::vector<std::string> {"8004", "103", "Kamal", "01-05-2026", "2"});
    Database::get_instance()->insert_row("RESERVATIONS", std::vector<std::string> {"8005", "301", "Nagata", "05-05-2026", "7"});
    Database::get_instance()->insert_row("RESERVATIONS", std::vector<std::string> {"8006", "101", "Burton", "10-05-2026", "1"});

    Database::get_instance()->create_table("SERVICES", 
        std::vector<std::string> {"SERVICE_ID", "NAME", "PRICE", "DEPT"}, 
        std::vector<std::string> {"NUMBER", "VARCHAR", "NUMBER", "VARCHAR"});
        
    Database::get_instance()->insert_row("SERVICES", std::vector<std::string> {"1", "ROOM_SERVICE", "45", "FOOD"});
    Database::get_instance()->insert_row("SERVICES", std::vector<std::string> {"2", "MINIBAR", "25", "FOOD"});
    Database::get_instance()->insert_row("SERVICES", std::vector<std::string> {"3", "LAUNDRY", "15", "MAINTENANCE"});
    Database::get_instance()->insert_row("SERVICES", std::vector<std::string> {"4", "SPA_MASSAGE", "120", "WELLNESS"});
    Database::get_instance()->insert_row("SERVICES", std::vector<std::string> {"5", "VALET_PARKING", "30", "FRONT_DESK"});
    Database::get_instance()->insert_row("SERVICES", std::vector<std::string> {"6", "AIRPORT_TAXI", "60", "CONCIERGE"});

    Database::get_instance()->create_table("BILLS", 
        std::vector<std::string> {"BILL_ID", "RES_ID", "AMOUNT", "PAID"}, 
        std::vector<std::string> {"NUMBER", "NUMBER", "NUMBER", "VARCHAR"});
        
    Database::get_instance()->insert_row("BILLS", std::vector<std::string> {"9901", "8001", "450", "YES"});
    Database::get_instance()->insert_row("BILLS", std::vector<std::string> {"9902", "8002", "1750", "NO"});
    Database::get_instance()->insert_row("BILLS", std::vector<std::string> {"9903", "8003", "35000", "NO"});
    Database::get_instance()->insert_row("BILLS", std::vector<std::string> {"9904", "8004", "400", "YES"});
    Database::get_instance()->insert_row("BILLS", std::vector<std::string> {"9905", "8005", "5600", "NO"});
    Database::get_instance()->insert_row("BILLS", std::vector<std::string> {"9905", "8005", "5600", "NO"});

    Interface* interface = Interface::get_instance();
    GLFWwindow* window = interface->init_window();
    interface->render(window);

    return 0;
}