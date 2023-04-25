#include <eosio/eosio.hpp>
#include <string>
#include <map>

using namespace eosio; 
using std::string;
using std::map;
using std::pair;

class [[eosio::contract]] supplychain : public contract
{
	public:
	   using contract::contract;

	   supplychain(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds) {}

            [[eosio::action]]
            void harvestitem(uint64_t _upc, uint64_t _sku, const name& _farmerid, string& _farmname, string& _farminformation, string& _farmlatitude, string& _farmlongitude, string& _productnotes){
		upsertitem(_upc,
                _sku,
                _farmerid,
                _farmerid,
                _farmname,
                _farminformation,
                _farmlatitude,
                _farmlongitude,
                _sku+_upc,
                _productnotes,
                0,
                "Harvested",
                _farmerid,
                _farmerid,
                _farmerid);
            };

            [[eosio::action]]   
            void processitem(uint64_t _upc, name caller){
		bool rolecheck = checkrole(caller, "Farmer");
		if (rolecheck) {
			require_auth(caller);
			bool stateharvested = checkitem(_upc, "Harvested");
			if (stateharvested) {
			   setstate(_upc, caller, "Processed");
			}			
		}
	    };

            [[eosio::action]]
            void packitem(uint64_t _upc, name caller){
		bool rolecheck = checkrole(caller, "Farmer");
		if (rolecheck) {
			require_auth(caller);
			bool stateprocessed = checkitem(_upc, "Processed");
			if (stateprocessed) {
			   setstate(_upc, caller, "Packed");
			}			
		}
            };

            [[eosio::action]]
            void sellitem(uint64_t _upc, name caller, uint64_t _price){
		bool rolecheck = checkrole(caller, "Farmer");
		if (rolecheck) {
			require_auth(caller);
			bool statepacked = checkitem(_upc, "Packed");
			if (statepacked) {
			   setstate(_upc, caller, "ForSale");
			   setprice(_upc, caller, _price);
			}			
		}
            };

            [[eosio::action]]
            void buyitem(uint64_t _upc, name caller){
		bool rolecheck = checkrole(caller, "Distributor");
		if (rolecheck) {
			require_auth(caller);
			bool stateforsale = checkitem(_upc, "ForSale");
			if (stateforsale) {
			   setstate(_upc, caller, "Sold");
			   setdist(_upc, caller);
			}			
		}
            };

            [[eosio::action]]
            void shipitem(uint64_t _upc, name caller){
		bool rolecheck = checkrole(caller, "Distributor");
		if (rolecheck) {
			require_auth(caller);
			bool statesold = checkitem(_upc, "Sold");
			if (statesold) {
			   setstate(_upc, caller, "Shipped");
			}			
		}
            };

            [[eosio::action]]
            void receiveitem(uint64_t _upc, name caller){
		bool rolecheck = checkrole(caller, "Retailer");
		if (rolecheck) {
			require_auth(caller);
			bool stateshipped = checkitem(_upc, "Shipped");
			if (stateshipped) {
			   setstate(_upc, caller, "Received");
			   setretailer(_upc, caller);
			}			
		}
            };

            [[eosio::action]]
            void purchaseitem(uint64_t _upc, name caller){
		bool rolecheck = checkrole(caller, "Consumer");
		if (rolecheck) {
			require_auth(caller);
			bool statereceived = checkitem(_upc, "Received");
			if (statereceived) {
			   setstate(_upc, caller, "Purchased");
			   setconsumer(_upc, caller);
			}			
		}

            };

            [[eosio::action]]
            void upsertitem(uint64_t upc,
                uint64_t sku,
                name ownerid,
                name farmerid,
                string farmname,
                string farminformation,
                string farmlatitude,
                string farmlongitude,
                uint64_t productid,
                string productnotes,
                uint64_t productprice,
                string state,
                name distributorid,
                name retailerid,
                name consumerid) {
		item_index items(get_self(), get_first_receiver().value);
		auto iterator = items.find(upc);
		if( iterator == items.end() ) {
		   items.emplace(farmerid, [&]( auto& row) {
			row.upc = upc;
			row.sku = sku;
			row.ownerid = ownerid;
			row.farmerid = farmerid;
			row.farmname = farmname;
			row.farminformation = farminformation;
			row.farmlatitude = farmlatitude;
			row.farmlongitude = farmlongitude;
			row.productid = productid;
			row.productnotes = productnotes;
			row.productprice = productprice;
			row.state = state;
			row.distributorid = distributorid;
			row.retailerid = retailerid;
			row.consumerid = consumerid;
		    });
		}
		else {
		   items.modify(iterator, farmerid, [&]( auto& row ) {
		   	row.upc = upc;
			row.sku = sku;
			row.ownerid = ownerid;
			row.farmerid = farmerid;
			row.farmname = farmname;
			row.farminformation = farminformation;
			row.farmlatitude = farmlatitude;
			row.farmlongitude = farmlongitude;
			row.productid = productid;
			row.productnotes = productnotes;
			row.productprice = productprice;
			row.state = state;
			row.distributorid = distributorid;
			row.retailerid = retailerid;
			row.consumerid = consumerid;
		   });	
		}
             };


	      [[eosio::action]]
              void setstate(uint64_t upc, name payingaccount, string state){
		item_index items(get_self(), get_first_receiver().value);
		auto iterator = items.find(upc);
		if( iterator == items.end() ) {
		   eosio::print_f("Item to modify not found");
		}
		else {
		   items.modify(iterator, payingaccount, [&]( auto& row ) {
			row.state = state;
		});
		}
	     };
		
	      [[eosio::action]]
              void setprice(uint64_t upc, name payingaccount, uint64_t price){
		item_index items(get_self(), get_first_receiver().value);
		auto iterator = items.find(upc);
		if( iterator == items.end() ) {
		   eosio::print_f("Item to modify not found");
		}
		else {
		   items.modify(iterator, payingaccount, [&]( auto& row ) {
			row.productprice = price;
		});
		}
	     };

	     [[eosio::action]]
             void setdist(uint64_t upc, name dist){
		item_index items(get_self(), get_first_receiver().value);
		auto iterator = items.find(upc);
		if( iterator == items.end() ) {
		   eosio::print_f("Item to modify not found");
		}
		else {
		   items.modify(iterator, dist, [&]( auto& row ) {
			row.distributorid = dist;
			row.ownerid = dist;
		});
		}
	     };



	     [[eosio::action]]
             void setconsumer(uint64_t upc, name consumer){
		item_index items(get_self(), get_first_receiver().value);
		auto iterator = items.find(upc);
		if( iterator == items.end() ) {
		   eosio::print_f("Item to modify not found");
		}
		else {
		   items.modify(iterator, consumer, [&]( auto& row ) {
			row.consumerid = consumer;
			row.ownerid = consumer;
		});
		}
	     };


	     [[eosio::action]]
             void setretailer(uint64_t upc, name retailer){
		item_index items(get_self(), get_first_receiver().value);
		auto iterator = items.find(upc);
		if( iterator == items.end() ) {
		   eosio::print_f("Item to modify not found");
		}
		else {
		   items.modify(iterator, retailer, [&]( auto& row ) {
			row.retailerid = retailer;
			row.ownerid = retailer;
		});
		}
	     };

            [[eosio::action]]
            void upsertrole(string role, name account_name){
		user_index users(get_self(), get_first_receiver().value);
		auto iterator = users.find(account_name.value);
		if( iterator == users.end() ) {
		   users.emplace(account_name, [&]( auto& row ) {
			row.account_name = account_name;
			row.role = role;
		   });
		}
		 else {
		 users.modify(iterator, account_name, [&]( auto& row ) {
			row.account_name = account_name;
			row.role = role;
		});
		}
	    };

            [[eosio::action]]
            bool checkitem(uint64_t upc, string state){
		item_index items(get_self(), get_first_receiver().value);
		auto itr = items.find(upc);
    		
		check(itr != items.end(), "item does not exist in table" );
                if (itr->state == state) {
			return true;
		} else {
			return false;
		}
            };

		

            [[eosio::action]]
            bool checkrole(name account_name, string role){
		user_index users(get_self(), get_first_receiver().value);
		auto itr = users.find(account_name.value);

		check(itr != users.end(), "user does not exist in table" );
		if (itr->role == role) {
			return true;
		} else {
			return false;
		}
            };

	private: 
		struct [[eosio::table]] user {
		    name account_name;
		    string role;
		    
	            uint64_t primary_key() const { return account_name.value;}
		};
		
		using user_index = eosio::multi_index<"users"_n, user>;

		struct [[eosio::table]] item {
	            uint64_t upc;
	            uint64_t sku;
	            name ownerid;
	            name farmerid;
	            string farmname;
	            string farminformation;
	            string farmlatitude;
	            string farmlongitude;
	            uint64_t productid;
	            string productnotes;
	            uint64_t productprice;
	            string state;
	            name distributorid;
	            name retailerid;
	            name consumerid;
		    uint64_t primary_key() const { return upc;}
	        };

		using item_index = eosio::multi_index<"items"_n, item>;

};
