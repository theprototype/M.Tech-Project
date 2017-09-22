# M.Tech-Project
Metaheuristic Algorithms for Winner Determination Problem in Combinatorial Auctions


## Representation

- Each Bid is defined using `struct` and contains sl_no, price, number of goods and list of Goods.
 ```
 struct Bid{
 	int sl_no;
 	double price;
 	int n;
 	struct node *goods;
 };
 ```
  - Goods are represented as linked list.
	```
	struct node{
		int good;
		struct node *link;
	};	
	```
