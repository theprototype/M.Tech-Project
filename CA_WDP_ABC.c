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

/* Global Declarations */

int no_of_bids,no_of_goods,no_of_dummy;

struct Bid *Bids; //Bids declared globally so that it is accessible any where
struct conflictSet* *conflictBids;
struct Solution *best_sol;
//struct Best_Solution{
//	struct Solution *sol;
//}

int get_num_from_string(char* my_string){
    int num=0;
    while(my_string[0]!=' '&&my_string[0]!='\n'&&my_string[0]!='\0'){
        num=(10*num)+my_string[0]-'0';
        ++my_string;
    }
    return num;
}

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

void copySolution(struct Solution *new_sol,struct Solution *solution){

    struct Bids *bids=solution->bids;
    new_sol->nBids=solution->nBids;
    new_sol->totalCost=solution->totalCost;
    struct Bids *new_sol_bids=new_sol->bids;
    while(bids){
        addBid(&new_sol->bids,bids->bid);
        bids=bids->link;
    }
}

struct Solution* unionSolution(struct Solution *solution){
    /*
        *Modify union,repair and improve function such that they create a new solution instead of modifing the existing solution.
        *CopySolution() - to get a new solution I am thinking to copy all the contents of solution to new solution and apply union to the new one.
    */
	struct Solution *new_sol=(struct Solution*)malloc(sizeof(struct Solution));
	struct Bids *bids=best_sol->bids;
	//struct Bids *headBid=solution->bids;
    copySolution(new_sol,solution);
	while(bids){
		struct Bids *temp=new_sol->bids;
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
			addBid(&(new_sol->bids),bids->bid);
            new_sol->totalCost+=bids->bid->price; //adding the bid price to the solution
			++(new_sol->nBids);
		}
		bids=bids->link;
	}
    return new_sol;
}

struct Solution* repairSolution(struct Solution *solution){
    struct Solution *new_sol=(struct Solution*)malloc(sizeof(struct Solution));
    copySolution(new_sol,solution);
    while(1){
        struct Bids *head=new_sol->bids;
        int n=new_sol->nBids;
        int *cnt=(int*)malloc(n*sizeof(int));
        float *cost=(float*)malloc(n*sizeof(float));
        memset(cnt,0,n*sizeof(int));
        int i=0;
        while(head){
            cost[i]=head->bid->price;
            struct conflictSet *conflicts=conflictBids[head->bid->sl_no];
            struct Bids *temp=new_sol->bids;
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
            break;
        //printf("\n%d %lf\n",j,max);
        head=new_sol->bids;
        struct Bids *prev=NULL;
        while(j--){
            prev=head;
            head=head->link;
        }
        if(head==new_sol->bids){
            new_sol->bids=head->link;
            new_sol->totalCost-=head->bid->price;
            --(new_sol->nBids);
        }
        else{
            prev->link=head->link;
            new_sol->totalCost-=head->bid->price;
            --(new_sol->nBids);
        }
    }
    //printf("\n%d\n",new_sol->nBids);
    return new_sol;
}

void improveSolution(struct Solution *solution,struct Bids *bidList){

	// not sure if it is completed or not
    struct Set *U=NULL;
    struct Bids *bids=bidList;
    struct Solution *new_sol=(struct Solution*)malloc(sizeof(struct Solution));
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

struct Solution* generate_random_solution(){
    int i;
    int *u=(int *)malloc(no_of_bids*sizeof(int));
    for(i=0;i<no_of_bids;++i)
		u[i]=i;
    struct Solution *solution=(struct Solution*)malloc(sizeof(struct Solution));
    solution->bids=NULL;
    shuffle(u,no_of_bids);
    for(i=0;i<no_of_bids;++i){
        if(insertBid(&solution->bids,&Bids[u[i]])){
            solution->totalCost+=Bids[u[i]].price;
            ++(solution->nBids);

        }

    }
    return solution;
}

struct Solution** generate_random_solutions(int ne){
    int i,j;
    struct Solution **solutions=(struct Solution**)malloc(ne*sizeof(struct Solution*));
	int *u=(int *)malloc(no_of_bids*sizeof(int));
	for(i=0;i<no_of_bids;++i){
		u[i]=i;
	}
	for(i=0;i<ne;++i){
        solutions[i]=(struct Solution*)malloc(sizeof(struct Solution));
		solutions[i]->bids=NULL;
		shuffle(u,no_of_bids);
		for(j=0;j<no_of_bids;++j){
			if(insertBid(&solutions[i]->bids,&Bids[u[j]])){
				solutions[i]->totalCost+=Bids[u[j]].price;
				++(solutions[i]->nBids);

			}

		}

	}
	/*for(i=0;i<ne;++i){
		printf("\n\n*******Solution : %d ********\n",i+1);
		displaySolution(&solutions[i]);

	}/**/
    return solutions;

}

struct Solution* find_best_sol(struct Solution *solutions,int ne){
    int i;
    struct Solution *best;
	best=&solutions[0];
    for(i=0;i<ne;++i){
		if(best->totalCost<solutions[i].totalCost){
			best=&solutions[i];
		}
		if(best->totalCost==solutions[i].totalCost){
			if(best->nBids>solutions[i].nBids)
				best=&solutions[i];
		}

	}
	//printf("\n\n********Best Solution *********\n");
	//displaySolution(best);
	return best;
}

struct Solution* generate_Neighbour_Solution(struct Solution *solution){
    struct Solution *new_sol=unionSolution(solution);
    //printf("\n%d\n",new_sol->nBids);
    new_sol=repairSolution(new_sol);
    //printf("\n\n\n\n new_sol\n");
    //displaySolution(new_sol);
    /*
        * need to add improveSolution() also
    */
    return new_sol;
}

int Select_and_Return_Index(int ne){


    //return an index between 0 to ne-1;
    return (int)(uol*ne);

}
void ABC(){
    int i;
    /*
        * Stoping condition can be in different ways.
        * stoping_count : no.of iterations
        * change of best_sol: if certain no.of iterations are completed without any change in best_sol
        * time.
    */
    int stoping_count=100;

	//generate ne random solutions
	int ne=4,no=4;
	int noimp=4;//no.of iterations without any update of the solution. Refer ABC algorithms for reference.
	int *changeCount=(int*)malloc(4*sizeof(int));
	memset(changeCount,0,ne*sizeof(int));
    struct Solution **solutions=generate_random_solutions(ne);
    best_sol=find_best_sol(*solutions,ne);
    while(--stoping_count){
        for(i=0;i<ne;++i){
            struct Solution *new_sol=generate_Neighbour_Solution(solutions[i]);
            //printf("\n%d\n",new_sol->nBids);
            if(new_sol->nBids==0){
                solutions[i]=generate_random_solution();
                changeCount[i]=0;
            }
            else if(new_sol->totalCost>solutions[i]->totalCost){
                solutions[i]=new_sol;
                changeCount[i]=0;
            }
            else if(changeCount[i]>=noimp){
                solutions[i]=generate_random_solution();
                changeCount[i]=0;
            }
            else{
                ++changeCount[i];
            }

            if(solutions[i]->totalCost>best_sol->totalCost)
                best_sol=solutions[i];
        }
        int *p=(int *)malloc(no*sizeof(int));
        struct Solution **S=(struct Solution**)malloc(no*sizeof(struct Solution*));
        for(i=0;i<no;++i){
            /* Implement
                for i:=1 to no do
                    pi := Select_and_Return_Index(E1, E2,….,Ene )
                    Si := Generate_Neighboring_Solution(E pi)
                    if (Si == ∅) then
                        artificially assign fitness worse than Epi to Si
                    if (Si is better than best_sol) then
                        best_sol := Si
                end for
            */
            //for Onlooker bees
            //How to select_and_return_Index()
            //Epi is solutions[p[i]]
            p[i]=Select_and_Return_Index(ne);
            S[i]=generate_Neighbour_Solution(solutions[p[i]]);
            if(S[i]->nBids==0){
                //artificially assign fitness worst than Epi to S[i]
                //S
            }
            if(S[i]->totalCost>best_sol->totalCost)
                best_sol=S[i];

        }
        for(i=0;i<no;++i){
            /* Implement
                for i:=1 to no do
                    if (Si is better than Epi )  then
                        Epi := Si
                end for
            */
            //Here Epi is solutions[p[i]]

            if(S[i]->totalCost>solutions[p[i]]->totalCost){
                solutions[p[i]]=S[i];
            }
        }
    }

}


int main(){
	//int no_of_bids;
	//int no_of_goods;
	int i,j;
	/*
        Code to skip first 20 lines in input
	*/
	int bytes_read;
    size_t nbytes = 100;
    char *my_string;

    /* These 2 lines are the heart of the program. */
    my_string = (char *) malloc (nbytes + 1);

    char str[200];
	bytes_read = getline (&my_string, &nbytes, stdin);
	while(my_string[0]=='%'||my_string[0]=='\n'){
		getline (&my_string, &nbytes, stdin);
	}
	while(my_string[0]!=' '){
        ++my_string;
	}++my_string;
    no_of_goods=get_num_from_string(my_string);


	srand( time(NULL));

    /*
        get goods,bids,dummy from input
        we are not using dummy yet
    */
	//scanf("%s %d",my_string,&no_of_goods);
	scanf("%s %d",my_string,&no_of_bids);
	scanf("%s %d",my_string,&no_of_dummy);
    struct Bids *bidsList=NULL;


	Bids=(struct Bid*)malloc(no_of_bids*sizeof(struct Bid));// Bids memory is allocated here
	conflictBids=(struct conflictSet**)malloc(no_of_bids*sizeof(struct conflictSet*));

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


    for(i=0;i<no_of_bids;++i){
        constructConflictSet(&conflictBids[i],&Bids[i],bidsList);
        //printf("\n\n********Conflict Set for Bid: %d",i);
        //displayConflictSet(conflictBids[i]);
    }
    ABC();
    //displaySolution(best_sol);
    printf("\nGoods :%d\n",no_of_goods);
	printf("Bids: %d\n",no_of_bids);
	printf("Dummy: %d\n",no_of_dummy);
    printf("\n\nBest:%lf\n\n",best_sol->totalCost);
    /*
    int n=4;
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
		}

	}
	printf("\n\n********Best Solution *********\n");
	displaySolution(best_sol);


	/*unionSolution(&solutions[0],best_sol);
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
