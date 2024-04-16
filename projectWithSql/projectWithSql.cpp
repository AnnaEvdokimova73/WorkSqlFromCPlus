#include <iostream>
#include <pqxx/pqxx>
#include "Windows.h"

#pragma execution_character_set("utf-8")

int main()
{
	setlocale(LC_ALL, "Russian");

	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	setvbuf(stdout, nullptr, _IOFBF, 1000);

	try
	{
		pqxx::connection conn(
			"host=localhost "
			"port=5432 "
			"dbname=clientsdb "
			"user=postgres "
			"password=123");

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

		// Insert data into tables
		tr.exec("Insert into client (client_name, client_surname, client_email)"
			" Values('Anna', 'Evdokimova', 'anna@mail.ru');");
		int id = tr.query_value<int>("Select client_id from client where client_name='Anna';");
		std::string execText = "Insert into phone (phone_client_id, phone_number) Values(" + std::to_string(id) + ", '79008007060'); ";
		tr.exec(execText);
		execText = "Insert into phone (phone_client_id, phone_number) Values(" + std::to_string(id) + ", '78005553535'); ";
		tr.exec(execText);

		tr.exec("Insert into client (client_name, client_surname, client_email)"
			" Values('Ivan', 'Ivanov', 'ivan@mail.ru');");
		id = tr.query_value<int>("Select client_id from client where client_surname='Ivanov';");
		execText = "Insert into phone (phone_client_id, phone_number) Values(" + std::to_string(id) + ", '79009009090'); ";
		tr.exec(execText);
		execText = "Insert into phone (phone_client_id, phone_number) Values(" + std::to_string(id) + ", '79999999999'); ";
		tr.exec(execText);

		tr.exec("Insert into client (client_name, client_surname, client_email)"
			" Values('Mark', 'Sweet', 'mark@gmail.com');");
		id = tr.query_value<int>("Select client_id from client where client_email='mark@gmail.com';");
		execText = "Insert into phone (phone_client_id, phone_number) Values(" + std::to_string(id) + ", '7777777777'); ";
		tr.exec(execText);
		execText = "Insert into phone (phone_client_id, phone_number) Values(" + std::to_string(id) + ", '88008008080'); ";
		tr.exec(execText);

		// Delete all client's phones
		//execText = "Delete from phone where phone_client_id=" + std::to_string(id);
		//tr.exec(execText);

		// Delete 1 client's phone
		execText = "Delete from phone where phone_client_id=" + std::to_string(2) + " AND phone_id=2";
		tr.exec(execText);

		// Updata client data
		tr.exec("Update client"
			" set client_surname = 'Petrova', client_email = 'petrova@mail.ru'"
			" where client_id = 1;");

		// Delete client
		tr.exec("Delete from phone where phone_client_id=1;");
		tr.exec("Delete from client where client_id=1;");

		//tr.commit();

		auto clientData = tr.query<std::string, std::string, std::string>("Select client_name, client_surname, client_email from client where client_name='Mark'");

		for (auto [name, surname, email] : clientData)
		{
			std::cout << "Found client: " << name << " " << surname << " with email " << email << std::endl;
		}
		tr.commit();
	}
	catch (pqxx::sql_error& err)
	{
		std::cout << err.what();
	}

	return 0;
}
