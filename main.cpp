// libpqxx.cpp : Defines the entry point for the application.
//

#include "main.h"
#include <pqxx/pqxx>

class ClientDB {
private:
	int id;
	std::string first_name;
	std::string last_name;
	std::string email;
	std::vector<std::string> phoneNumber;

	std::unique_ptr<pqxx::connection> connection;
private:
	void addConnection() 
	{
			connection = std::make_unique<pqxx::connection>(
				"host=localhost "
				"port=5432 "
				"dbname=dataclients "
				"user=postgres "
				"password=Rus463812"
			);

			
	}
public:
	ClientDB() = default;

	void connect()
	{
		addConnection();
	}

	void prepareStatements()
	{
		connection->prepare("insert_client", "INSERT INTO Clients (first_name, last_name, email) VALUES ($1, $2, $3)");
		connection->prepare("insert_phone", "INSERT INTO Phones (client_id, phone_number) VALUES ($1, $2)");
		connection->prepare("update_client", "UPDATE Clients SET first_name = $2, last_name = $3, email = $4 WHERE id = $1");
		connection->prepare("delete_all_phones", "DELETE FROM Phones WHERE client_id = $1");
		connection->prepare("delete_phone", "DELETE FROM Phones WHERE client_id = $1 AND phone_number = $2");
		connection->prepare("delete_client", "DELETE FROM Clients WHERE id = $1");
		connection->prepare("find_client", R"(SELECT c.first_name f, c.last_name l, c.email e, p.phone_number pn FROM Clients c 
				LEFT JOIN Phones p ON c.id = p.client_id
				WHERE ($1 = '' OR c.first_name = $1) AND ($2 = '' OR c.last_name = $2) AND ($3 = '' OR c.email = $3) AND ($4 = '' OR p.phone_number = $4);)");
		connection->prepare("multi_client", R"(SELECT c.first_name f, c.last_name l, c.email e, p.phone_number pn FROM Clients c 
				LEFT JOIN Phones p ON c.id = p.client_id
				WHERE c.first_name = $1 OR c.last_name = $2 OR c.email = $3 OR p.phone_number = $4;)");
	};

	void createTables()
	{
		pqxx::work tx(*connection);

		tx.exec(R"(
        CREATE TABLE IF NOT EXISTS Clients (
            id SERIAL PRIMARY KEY,
            first_name VARCHAR(100) NOT NULL,
            last_name VARCHAR(100) NOT NULL,
            email VARCHAR(200) NOT NULL
        );
    )");

		tx.exec(R"(
        CREATE TABLE IF NOT EXISTS Phones (
            id SERIAL PRIMARY KEY,
            client_id INTEGER NOT NULL REFERENCES Clients(id),
            phone_number VARCHAR(20) NOT NULL
        );
    )");

		tx.commit();
	}

	void addClient(const std::string& first_name, const std::string& last_name, const std::string& email)
	{
		pqxx::work tx(*connection);
		tx.exec_prepared("insert_client", first_name, last_name, email);

		tx.commit();
	}

	void addPhone(int id_client, const std::string& Phones)
	{
		pqxx::work tx(*connection);

		tx.exec(
			pqxx::prepped{ "insert_phone" },
			pqxx::params{ id_client, Phones }
			);
		tx.commit();
	}

	void updateClient(int id, const std::string& first_name, const std::string& last_name, const std::string& email)
	{
		pqxx::work tx(*connection);
		tx.exec(
			pqxx::prepped{ "update_client" },
			pqxx::params{ id, first_name, last_name, email }
		);
			tx.commit();
	}

	void deleteAllPhones(int id_client)
	{
		pqxx::work tx(*connection);
		tx.exec(
			pqxx::prepped{ "delete_all_phones" },
			pqxx::params{id_client}
		);
		tx.commit();
	}

	void deletePhone(int client_id, const std::string& phone)
	{
		pqxx::work tx(*connection);
		tx.exec(
			pqxx::prepped{ "delete_phone" },
			pqxx::params{ client_id, phone }
		);
		tx.commit();
	}

	void deleteClient(int id)
	{
		pqxx::work tx(*connection);
		tx.exec(
			pqxx::prepped{ "delete_all_phones" },
			pqxx::params{ id }
			);
		tx.exec(
			pqxx::prepped{ "delete_client" },
			pqxx::params{ id }
		);
		tx.commit();
	}

	void findClient(const std::string& first_name = "", const std::string& last_name = "", const std::string& email = "", const std::string& phone_number = "")
	{
		if (first_name.empty() && last_name.empty() && email.empty() && phone_number.empty())
		{
			std::cout << "No search parameters!\n";
			return;
		}
		pqxx::work tx(*connection);
		pqxx::result result = tx.exec(
			pqxx::prepped{ "find_client" },
			pqxx::params{ first_name, last_name, email, phone_number }
		);

		if (result.empty())
		{
			std::cout << "Client was not found!\n";
			return;
		}

		for (const auto& row : result)
		{
			std::cout << row[0].as<std::string>() << ' '
				<< row[1].as<std::string>() << '\n'
				<< "email: " << row[2].as<std::string>() << '\n';

			if (!row[3].is_null())
			{
				std::cout << "Phone number: " << row[3].as<std::string>() << '\n';
			}
			else { std::cout << "Number was not found!\n"; }
		}
	}

	void multiClient(const std::string& first_name = "", const std::string& last_name= "", const std::string& email = "", const std::string& phone_number = "")
	{
		if (first_name.empty() && last_name.empty() && email.empty() && phone_number.empty())
		{
			std::cout << "No search parameters!\n";
			return;
		}

		pqxx::work tx(*connection);
		pqxx::result result = tx.exec(
			pqxx::prepped{ "multi_client" },
			pqxx::params{ first_name, last_name, email, phone_number }
		);
		
		if (result.empty())
		{
			std::cout << "Client was not found!\n";
			return;
		}

		for (const auto& row : result)
		{
			std::cout << row[0].as<std::string>() << ' '
				<< row[1].as<std::string>() << '\n'
				<< "email: " << row[2].as<std::string>() << '\n';

			if (!row[3].is_null())
			{
				std::cout << "Phone number: " << row[3].as<std::string>() << '\n';
			}
			else { std::cout << "Number was not found!\n"; }
		}
	}
};

int main()
{
	
	try
	{
		ClientDB db{};
		db.connect();
		db.createTables();
		db.prepareStatements();

		db.addClient("Igor", "Ravazyan", "igor@mail.ru");
		db.addClient("Roman", "Taa", "taa@mail.ru");
		
		db.addPhone(1, "+79895674327");
		db.addPhone(2, "+89006743890");
		db.addPhone(1, "+346505");

		std::cout << "\n\nData of Igor\n";
		db.findClient("Igor", "", "", "");

		std::cout << "\n\nData of Tamara\n";
		db.updateClient(1, "Tamara", "Boykova", "boykova@yandex.ru");
		db.deleteAllPhones(1);
		db.addPhone(1, "+79003456789");
		db.addPhone(1, "+79009876543");
		db.findClient("", "Boykova", "", "");

		std::cout << "\n\nMultiFindClients:\n";
		db.multiClient("Roman", "", "", "+79009876543");

		std::cout << "\n\nDelete and find Tamara:\n";
		db.deleteClient(1);
		db.findClient("", "Boykova", "", "");
	}
	catch (pqxx::sql_error& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << '\n';
	}
	return EXIT_SUCCESS;
}