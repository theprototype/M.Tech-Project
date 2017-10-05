#include<stdio.h>
#include<stdlib.h>
#include<string.h>
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
	struct conflictSet *cs;
};

struct node{
	int good;
	struct node *link;
};

struct conflictSet{
	struct Bid *bid;
	struct conflictSet *link;
};

// need to remove Bids and conflictSet. Use Set in that place
struct Set{
    struct Bid *bid;
    struct Set *link;

};


//struct Best_Solution{
//	struct Solution *sol;
//}

int checkForConflict(struct Bid *bid1,struct Bid *bid2){ // returns 1 if there is a conflcit.

	struct node *head1=bid1->goods;

	while(head1){
		struct node *head2=bid2->goods;
		while(head2){
			//printf("%d  %d\n",head1->good,head2->good);
			if(head1->good==head2->good){

				return 1;
			}
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

struct conflictSet* createConflictNode(struct Bid *bid){
    struct conflictSet* temp=(struct conflictSet*)malloc(sizeof(struct conflictSet));
    temp->bid=bid;
    temp->link=NULL;
    return temp;
}


void constructConflictSet(struct conflictSet **root,struct Bid *bid,struct Bids *bids){


        struct Bids *head=bids;
        while(head){
            if(bid==(head->bid)){
                head=head->link;
                continue;
            }
			//int flag=checkForConflict(head->bid,bid);
			//printf("\n\n%d\n\n",flag);
			if(checkForConflict(head->bid,bid)){
				//printf("\nYes\n");
				struct conflictSet *temp=createConflictNode(head->bid);
				if(*root==NULL){
                    *root=temp;
				}
				else{
                    temp->link=*root;
                    *root=temp;
				}
			}
			head=head->link;
		}
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
	printf("\nNo.of Bids in the solution: %d\n",solution->nBids);
	int i;
	struct Bids *bids=solution->bids;
	while(bids){
		struct Bid *bid=bids->bid;
		displayBid(bid);
		bids=bids->link;
	}


}

void displayConflictSet(struct conflictSet *root){
    struct conflictSet *bids=root;
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

void addBid(struct Bids **root,struct Bid *bid){
	struct Bids *temp=createBidNode(bid);
	if(*root==NULL)
		*root=temp;
	else{
		temp->link=(*root);
		*root=temp;
	}

}

void unionSolution(struct Solution *solution,struct Solution *best_sol){
	struct Bids *bids=best_sol->bids;
	//struct Bids *headBid=solution->bids;
	while(bids){
		struct Bids *temp=solution->bids;
		int flag=1;
		while(temp){
			if(bids->bid==temp->bid){
				//printf("here");
				//printf("\n%p %p\n",bids->bid,temp->bid);
				flag=0;//bid already in the set
				break;
			}
			temp=temp->link;
		}
		if(flag){
			//printf("here");
			addBid(&(solution->bids),bids->bid);
            solution->totalCost+=bids->bid->price; //adding the bid price to the solution
			++(solution->nBids);
		}
		bids=bids->link;
	}

}


void repairSolution(struct Solution *solution,struct conflictSet **conflictBids){
    while(1){
        struct Bids *head=solution->bids;
        int n=solution->nBids;
        int *cnt=(int*)malloc(n*sizeof(int));
        float *cost=(float*)malloc(n*sizeof(float));
        memset(cnt,0,n*sizeof(int));
        int i=0;
        while(head){
            cost[i]=head->bid->price;
            struct conflictSet *conflicts=conflictBids[head->bid->sl_no];
            struct Bids *temp=solution->bids;
            while(temp){
                struct conflictSet* tconflicts=conflicts;
                while(tconflicts){
                    if(temp->bid==tconflicts->bid){
                        ++cnt[i];
                    }
                    tconflicts=tconflicts->link;
                }
                temp=temp->link;
            }
            head=head->link;
            ++i;
        }
        /*for(i=0;i<n;++i){
            printf("   %d\n",cnt[i]);
        }*/
        double max=0;
        int j=-1;
        for(i=0;i<n;++i){
            double probabilty=cnt[i]/cost[i];
            if(probabilty>max){
                j=i;
                max=probabilty;
            }
        }
        if(max==0)
            return;
        //printf("\n%d %lf\n",j,max);
        head=solution->bids;
        struct Bids *prev=NULL;
        while(j--){
            prev=head;
            head=head->link;
        }
        if(head==solution->bids){
            solution->bids=head->link;
            solution->totalCost-=head->bid->price;
            --(solution->nBids);
        }
        else{
            prev->link=head->link;
            solution->totalCost-=head->bid->price;
            --(solution->nBids);
        }
    }
}

void improveSolution(struct Solution *solution,struct Bids *bidList){
    struct Set *U=NULL;
    struct Bids *bids=bidList;
    while(bids){
        int flag=1;
        struct Bids *head=solution->bids;
        while(head){
            if(bids->bid==head->bid||checkForConflict(bids->bid,head->bid)){
                flag=0;
                head=head->link;
                break;
            }

            head=head->link;
        }
        if(flag){
            struct Set *temp=(struct Set *)malloc(sizeof(struct Set));
            temp->bid=bids->bid;
            temp->link=NULL;
            if(U==NULL){
                U=temp;
            }
            else{
                temp->link=U;
                U=temp;
            }

        }
        bids=bids->link;
    }
    /*printf("\n\n\n\n");
    int c=0;
    while(U){
        displayBid(U->bid);
        //printf("\n\n\n      %d\n\n\n",++c);
        U=U->link;
    }
    /**/
}



int main(){
	int no_of_bids;
	int no_of_goods;
	srand( time(NULL));
	scanf("%d %d",&no_of_bids,&no_of_goods);
    struct Bids *bidsList=NULL;
	struct Bid *Bids=(struct Bid*)malloc(no_of_bids*sizeof(struct Bid));
	struct conflictSet* *conflictBids=(struct conflictSet**)malloc(no_of_bids*sizeof(struct conflictSet*));
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

	for(i=0;i<no_of_bids;++i){
        struct Bids *temp=createBidNode(&Bids[i]);
        if(bidsList==NULL){
            bidsList=temp;
        }
        else{
            temp->link=bidsList;
            bidsList=temp;
        }
	}


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
    for(i=0;i<no_of_bids;++i){
        constructConflictSet(&conflictBids[i],&Bids[i],bidsList);
        printf("\n\n********Conflict Set for Bid: %d",i);
        displayConflictSet(conflictBids[i]);
    }



	for(i=0;i<n;++i){
		printf("\n\n*******Solution : %d ********\n",i+1);
		displaySolution(&solutions[i]);

	}
	struct Solution *best_sol;
	best_sol=&solutions[0];
	for(i=0;i<n;++i){
		if(best_sol->totalCost<solutions[i].totalCost){
			best_sol=&solutions[i];
		}
		if(best_sol->totalCost==solutions[i].totalCost){
			if(best_sol->nBids>solutions[i].nBids)
				best_sol=&solutions[i];
		}/**/

	}
	printf("\n\n********Best Solution *********\n");
	displaySolution(best_sol);
	/*for(i=0;i<no_of_bids;++i){
		printf("\n%p\n",&Bids[i]);
	}
	/*printf("\n\n\n");
	struct Bids *bids=best_sol->bids;
	while(bids){
		struct Bid *bid=bids->bid;
		printf("\n%p\n",bid);
		displayBid(bid);
		bids=bids->link;
	}/**/

	unionSolution(&solutions[0],best_sol);
	printf("\n\n********After Union *********\n");
	displaySolution(&solutions[0]);


    repairSolution(&solutions[0],conflictBids);
    printf("\n\n********After Repair *********\n");
    displaySolution(&solutions[0]);



    struct Solution *dummySolution=(struct Solution*)malloc(sizeof(struct Solution));
    dummySolution->bids=createBidNode(&Bids[5]);
    improveSolution(dummySolution,bidsList);

	/**/
}
