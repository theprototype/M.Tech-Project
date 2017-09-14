#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define uol ((rand())/(RAND_MAX+1.0))

struct Solution{
	double totalCost;
	int nBids;
	struct Bids *bids;
};

struct Bids{
	struct Bid *bid;
	struct Bids *link;

};
struct Bid{
	int sl_no;
	double price;
	int n;
	struct node *goods;
};

struct node{
	int good;
	struct node *link;
};

int checkForConflict(struct Bid *bid1,struct Bid *bid2){
	
	struct node *head1=bid1->goods;
	
	while(head1){
		struct node *head2=bid2->goods;
		while(head2){
			//printf("%d  %d\n",head1->good,head2->good);
			if(head1->good==head2->good)
				return 1;
			head2=head2->link;
		}
		head1=head1->link;
	}
	return 0;
}

struct Bids* createBidNode(struct Bid *bid){
	struct Bids *temp=(struct Bids*)malloc(sizeof(struct Bids));
	temp->bid=bid;
	temp->link=NULL;
}
void displayGoods(struct node *root){
	printf("\nGoods:");	
	while(root!=NULL){
		printf("%d ",root->good);
		root=root->link;
	}
	printf("\n");
}
void displayBid(struct Bid *bid){
	printf("\nBid Price: %lf",bid->price);
	printf("\nNo.of Goods in the Bid: %d",bid->n);
	displayGoods(bid->goods);

}

int insertBid(struct Bids **root,struct Bid *bid){
	struct Bids *temp=createBidNode(bid);
	if(*root==NULL)
		*root=temp;
	else{
		struct Bids *head=*root,*prev=NULL;
		while(head){
			int flag=checkForConflict(head->bid,bid);
			//printf("\n\n%d\n\n",flag);
			if(checkForConflict(head->bid,bid)){
				//printf("\nYes\n");
				return 0;
			}
			prev=head;
			head=head->link;
		}
		prev->link=temp;
		//displayBid(temp->bid);
	}
	return 1;
}





struct node* createGood(int data){
	struct node *temp=(struct node*)malloc(sizeof(struct node));
	temp->good=data;
	temp->link=NULL;
	return temp;
}

void insertGood(struct node **root,int data){
	struct node *temp=createGood(data);		
	if(*root==NULL)
		*root=temp;
	else{
		temp->link=(*root);
		*root=temp;
	}
}


/*void displayGoods(struct node *root){
	printf("\nGoods:");	
	while(root!=NULL){
		printf("%d ",root->good);
		root=root->link;
	}
	printf("\n");
}

/*void displayBid(struct Bid *bid){
	printf("\nBid Price: %lf",bid->price);
	printf("\nNo.of Goods in the Bid: %d",bid->n);
	displayGoods(bid->goods);

}*/

void displaySolution(struct Solution *solution){
	printf("\nTotal Cost: %lf",solution->totalCost);
	printf("\nNo.of Bids in the solution: %d",solution->nBids);
	int i;
	struct Bids *bids=solution->bids;
	while(bids){
		struct Bid *bid=bids->bid;
		displayBid(bid);
		bids=bids->link;
	}
	

}

void shuffle(int *u,int n){
	int i;	
	for(i=0;i<n;++i){
		int j=(int)(uol*n);
		int temp=u[i];
		u[i]=u[j];
		u[j]=temp;

	}

}


int main(){
	int no_of_bids;	
	int no_of_goods;
	srand( time(NULL));
	scanf("%d %d",&no_of_bids,&no_of_goods);
	struct Bid *Bids=(struct Bid*)malloc(no_of_bids*sizeof(struct Bid));
	int i,j;
	for(i=0;i<no_of_bids;++i){
		//int temp;
		scanf("%d",&Bids[i].sl_no);
		scanf("%lf",&Bids[i].price);
		Bids[i].goods=NULL;
		char good;
		int count=0,space=0;
		
		scanf("%c",&good);
		
		while(good!='#'){
			int flag=0,res=0;
			if(good==' ')
				space++;
			while(isdigit(good)){
				flag=1; //to know if we have to add to bid
				res=res*10+good-'0'; //for more than one digit number
				scanf("%c",&good);
			}
			
			if(flag){
				insertGood(&Bids[i].goods,res);
				++count;

			}
			scanf("%c",&good);
		}
		Bids[i].n=count;
	}	
	//printf("\n\n\n\n");
	/*for(i=0;i<no_of_bids;++i){
		printf("%d %d %lf ",i,Bids[i].n,Bids[i].price);
		displayGoods(Bids[i].goods);
		printf("\n");
	}*/
	int n=4;
	struct Solution *solutions=(struct Solution*)malloc(n*sizeof(struct Solution));
	int *u=(int *)malloc(no_of_bids*sizeof(int));
	for(i=0;i<no_of_bids;++i){
		u[i]=i;
	}
	for(i=0;i<n;++i){
		solutions[i].bids=NULL;
		shuffle(u,no_of_bids);
		for(j=0;j<no_of_bids;++j){
			if(insertBid(&solutions[i].bids,&Bids[u[j]])){
				solutions[i].totalCost+=Bids[u[j]].price;
				++(solutions[i].nBids);
				
			}
			
		}		
	}/**/
	for(i=0;i<n;++i){
		printf("\n\n*******Solution : %d ********\n",i+1);
		displaySolution(&solutions[i]);

	}
	
}
