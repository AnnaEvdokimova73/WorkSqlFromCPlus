#include <iostream>
#include <pqxx/pqxx>
#include "Windows.h"

#pragma execution_character_set("utf-8")

class DataBaseManager {
public:
	DataBaseManager() : conn("host=localhost "
							 "port=5432 "
							 "dbname=clientsdb "
							 "user=postgres "
							 "password=123")
	{}

	void createDbStructure()
	{
		pqxx::transaction tr{ conn };
		tr.exec("Drop table phone;");
		tr.exec("Drop table client;");

		// Create tables
		tr.exec("Create table if not exists client ("
			    " client_id serial primary key,"
			    " client_name varchar(250) not null,"
			    " client_surname varchar(250) not null,"
			    " client_email varchar(250) not null"
			    " ); ");

		tr.exec("Create table if not exists phone ("
			    " phone_id serial primary key,"
			    " phone_client_id int references client(client_id),"
			    " phone_number varchar(250) unique"
			    " ); ");
		tr.commit();
	}

	int addNewClient(const std::string name, const std::string surname, const std::string email)
	{
		pqxx::transaction tr{ conn };

		std::string execText = "Insert into client (client_name, client_surname, client_email)"
			" Values('" + name + "', '" + surname + "', '" + email + "');";
		tr.exec(execText);
		execText = "Select client_id from client where client_name='" + name + "';";
		int id = tr.query_value<int>(execText);

		tr.commit();

		return id;
	}

	int addPhone(const int clientId, const std::string phone)
	{
		pqxx::transaction tr{ conn };

		std::string execText = "Insert into phone (phone_client_id, phone_number)"
			" Values(" + std::to_string(clientId) + ", '" + phone + "'); ";
		tr.exec(execText);
		execText = "Select phone_id from phone where phone_number='" + phone + "';";
		int id = tr.query_value<int>(execText);

		tr.commit();
		return id;
	}

	void updateClientSurname(const int clientId, const std::string surname)
	{
		pqxx::transaction tr{ conn };

		std::string execText = "Update client"
			" set client_surname = '" + surname + "'"
			" where client_id =" + std::to_string(clientId)  + "; ";
		tr.exec(execText);

		tr.commit();
	}

	void updateClientEmail(const int clientId, const std::string email)
	{
		pqxx::transaction tr{ conn };

		std::string execText = "Update client"
			" set client_email = '" + email + "'"
			" where client_id =" + std::to_string(clientId) + "; ";
		tr.exec(execText);

		tr.commit();
	}

	void deletePhone(const int clientId, const int phoneId)
	{
		pqxx::transaction tr{ conn };

		std::string execText = "Delete from phone where phone_client_id=" + std::to_string(clientId) + " AND phone_id=" + std::to_string(phoneId);
		tr.exec(execText);

		tr.commit();
	}

	void deleteClient(const int clientId)
	{
		pqxx::transaction tr{ conn };

		std::string execText = "Delete from phone where phone_client_id=" + std::to_string(clientId) + "; ";
		tr.exec(execText);
		execText = "Delete from client where client_id=" + std::to_string(clientId) + "; ";
		tr.exec(execText);

		tr.commit();
	}

	std::tuple<std::string, std::string, std::string> getClientByName(const std::string name)
	{
		pqxx::transaction tr{ conn };

		std::string execText = "Select client_name, client_surname, client_email from client where client_name='" + name + "'";
		auto clientData = tr.query<std::string, std::string, std::string>(execText);

		std::tuple<std::string, std::string, std::string> clientTuple;
		for (auto [name, surname, email] : clientData)
		{
			clientTuple = std::make_tuple(name, surname, email);
		}

		tr.commit();
		return clientTuple;
	}

	std::tuple<std::string, std::string, std::string> getClientBySurname(const std::string surname)
	{
		pqxx::transaction tr{ conn };

		std::string execText = "Select client_name, client_surname, client_email from client where client_surname='" + surname + "'";
		auto clientData = tr.query<std::string, std::string, std::string>(execText);

		std::tuple<std::string, std::string, std::string> clientTuple;
		for (auto [name, surname, email] : clientData)
		{
			clientTuple = std::make_tuple(name, surname, email);
		}

		tr.commit();
		return clientTuple;
	}

	std::tuple<std::string, std::string, std::string> getClientByEmail(const std::string email)
	{
		pqxx::transaction tr{ conn };

		std::string execText = "Select client_name, client_surname, client_email from client where client_email='" + email + "'";
		auto clientData = tr.query<std::string, std::string, std::string>(execText);

		std::tuple<std::string, std::string, std::string> clientTuple;
		for (auto [name, surname, email] : clientData)
		{
			clientTuple = std::make_tuple(name, surname, email);
		}

		tr.commit();
		return clientTuple;
	}

private:
	pqxx::connection conn;
};

int main()
{
	setlocale(LC_ALL, "Russian");

	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	setvbuf(stdout, nullptr, _IOFBF, 1000);

	try
	{
		DataBaseManager dbManager;
		int clientId, phoneId;

		dbManager.createDbStructure();

		clientId = dbManager.addNewClient("Anna", "Evdokimova", "anna@mail.ru");
		dbManager.addPhone(clientId, "79008007060");
		dbManager.addPhone(clientId, "79998887766");

		clientId = dbManager.addNewClient("Ivan", "Ivanov", "ivan@mail.ru");
		dbManager.addPhone(clientId, "79000000000");
		dbManager.addPhone(clientId, "79999999999");
		dbManager.addPhone(clientId, "78888888888");

		clientId = dbManager.addNewClient("Mark", "Sweet", "sweet@google.com");
		dbManager.addPhone(clientId, "35999567390");
		phoneId = dbManager.addPhone(clientId, "36789028827");

		dbManager.deletePhone(clientId, phoneId); // Удаляем второй номер Марка

		dbManager.deleteClient(2); // Удаляем Ивана

		clientId = dbManager.addNewClient("Gali", "Letova", "gali@mail.ru");
		dbManager.addPhone(clientId, "567390289028");
		dbManager.addPhone(clientId, "345678928920");
		dbManager.addPhone(clientId, "274904874029");

		dbManager.updateClientSurname(clientId, "Sweet");
		dbManager.updateClientEmail(clientId, "gali@google.com");

		auto [name, surname, email] = dbManager.getClientByName("Mark");
		std::cout << "Found client1: " << name << " " << surname << " with email " << email << std::endl;

		auto [name2, surname2, email2] = dbManager.getClientBySurname("Evdokimova");
		std::cout << "Found client2: " << name2 << " " << surname2 << " with email " << email2 << std::endl;

		auto [name3, surname3, email3] = dbManager.getClientByEmail("gali@google.com");
		std::cout << "Found client3: " << name3 << " " << surname3 << " with email " << email3 << std::endl;
	}
	catch (pqxx::sql_error& err)
	{
		std::cout << err.what();
	}

	return 0;
}
