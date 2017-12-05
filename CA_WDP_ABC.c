#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#define uol ((rand())/(RAND_MAX+1.0))
#define NE 1000 //Number of Employee bees 125
#define NO 400 //Number of Outlooker bees 125
#define ABC_ITERATIONS 250 //Number of Iterations for ABC algorithm 350,700
#define LIMIT_OF_ITERATIONS_WITH_NOCHANGE 10 //60 limiting the no.of iteration a solution can be without any change. After this limit we assign a random solution to it.
#define SEED 1
#define PROB_TAKE_FROM_SOL 0.75 //0.75                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  //0.75
#define PROB_TAKE_FROM_BEST 0.5 //0.5
#define REPAIR 1

#define ITERATE 30 // iterate by changing randoms (SEED) . we take the best of all these iteration


struct Solution{// for Solution
	double totalCost;// total cost of the solution. This is the fitness value too. We want this to be highest.
	int nBids;// numbers of bids in the solution
	struct BidsSet *bids;// pointer to the bids that belongs to the solution
};

struct BidsSet{// Set of bids represented as linked list

	struct Bid *bid;
	struct BidsSet *link;

};

struct Bid{// For Bid
	int sl_no;// serial number of a bid as it is given in input
	double price;// price the bid is offering
	int n;// number of goods in the bid including the dummy goods
	struct node *goods;// set of goods represented as linked list
	struct conflictSet *cs;// not used. For future use if needed. No implementation till now
	int *goods_binary;//binary representation of goods in a bid.
};

struct node{// for Good
	int good;// good number
	struct node *link;
};

struct conflictSet{// same as BidsSet. In future will have only one representation
	struct Bid *bid;
	struct conflictSet *link;
};

// TODO need to remove Bids and conflictSet. Use Set in that place
struct Set{// same as BidsSet and conflictSet.
    struct Bid *bid;
    struct Set *link;

};

struct ListCntCost{

    int cnt;
    float cost;
    struct ListCntCost *link;
};

/* Global Declarations */

int no_of_bids,no_of_goods,no_of_dummy;

struct Bid *Bids; //Bids declared globally so that it is accessible any where
struct BidsSet *bidList=NULL;
struct conflictSet* *conflictBids;
struct Solution *gbest_sol=NULL;
struct Solution *best_sol=NULL;
double probabilty_take_from_solution=PROB_TAKE_FROM_SOL; // changing this makes the program slow don't know why
double probabilty_take_from_best=PROB_TAKE_FROM_BEST; // changing this makes the program slow don't know why

struct ListCntCost* insertCntCostatTail(struct ListCntCost **root,struct ListCntCost *tail,int cnt,int cost){
    struct ListCntCost *temp=(struct ListCntCost*)malloc(sizeof(struct ListCntCost));
    temp->cnt=cnt;
    temp->cost=cost;
    temp->link=NULL;
    if(*root==NULL){
        *root=temp;
    }
    else{
        tail->link=temp;
    }
    return temp;
}

int* get_binary_goods_for_Solution(struct Solution *solution){ // binary representation of goods in a solution. Using for debuggin purpose only. Not used in any of the implementation.
    //Used to check if any conflict bids are added by mistake to the solution. Only for debug purpose.

    int *binary_goods_in_Solution=(int*)calloc(no_of_goods+no_of_dummy,sizeof(int));// in addition to the original goods there are dummy goods too which will be part of solution so, the size is no_of_goods+no_of_dummy
    //memset(binary_goods_in_Solution,0,(no_of_goods+no_of_dummy)*sizeof(int));
    struct BidsSet *bids=solution->bids;
    while(bids){ //iterating through all the bids in the solution
        struct node *goods=bids->bid->goods;
        while(goods){ //iterating through all the goods in each bid
            ++(binary_goods_in_Solution[goods->good]); //incrementing the value if a good is present
            goods=goods->link;
        }

        bids=bids->link;
    }
    return binary_goods_in_Solution;
}

int get_num_from_string(char* my_string){//used when a number is represented in string. converts to int type and returns
    //using because I am skipping unwanted content from the input file.To do so I am checking if first char of each line is if '%' or not.
    //I am storing the line in string using getline. when the actual input comes it is also stored in the string, so to get the number from string I am using this.
    int num=0;
    while(my_string[0]!=' '&&my_string[0]!='\n'&&my_string[0]!='\0'){
        num=(10*num)+my_string[0]-'0';
        ++my_string;
    }
    return num;
}

int checkForConflict(struct Bid *bid1,struct Bid *bid2){ // returns 1 if there is a conflict.
    //Used previously when binary representation of bids is not used.
    //

	struct node *head1=bid1->goods;

	while(head1){// iterating through each good of bid1
		struct node *head2=bid2->goods;
		while(head2){// iterating through each the good of bid2

			if(head1->good==head2->good){

				return 1;
			}
			head2=head2->link;
		}
		head1=head1->link;
		free(head2);
	}
	free(head1);
	return 0;
}

int checkForConflict_usingBinary(struct Bid *bid1,struct Bid *bid2){// returns 1 if there is a conflict
    int i;
    struct node *b1Goods=bid1->goods;
    while(b1Goods){//iterating through each good in bid1
        if(bid2->goods_binary[b1Goods->good]==1) // checking if that good is also present in bid2
            return 1;
        b1Goods=b1Goods->link;
    }
    return 0;
}

struct BidsSet* createBidNode(struct Bid *bid){// Creating BidNode to link it to Set
	struct BidsSet *temp=(struct BidsSet*)malloc(sizeof(struct BidsSet));
	temp->bid=bid;
	temp->link=NULL;
	return temp;
}

void displayGoods(struct node *root){ //Used for debug purpose
	printf("\nGoods:");
	while(root!=NULL){
		printf("%d ",root->good);
		root=root->link;
	}
	printf("\n");
}

void displayBid(struct Bid *bid){ //Used for debug purpose
    printf("\nBid sl_No:%d",bid->sl_no);
	printf("\nBid Price: %lf",bid->price);
	printf("\nNo.of Goods in the Bid: %d",bid->n);
	displayGoods(bid->goods);

}

int insertBid(struct BidsSet **root,struct Bid *bid){// inserts Bid if not conflict with already present bids in the solution
	struct BidsSet *temp=createBidNode(bid);
	if(*root==NULL)
		*root=temp;
	else{
		struct BidsSet *head=*root,*prev=NULL;
		while(head){
			//int flag=checkForConflict(head->bid,bid);
			//printf("\n\n%d\n\n",flag);
			if(checkForConflict_usingBinary(head->bid,bid)){
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

struct conflictSet* createConflictNode(struct Bid *bid){ //same as creating a bidset node
    struct conflictSet* temp=(struct conflictSet*)malloc(sizeof(struct conflictSet));
    temp->bid=bid;
    temp->link=NULL;
    return temp;
}

void constructConflictSet(struct conflictSet **root,struct Bid *bid,struct BidsSet *bids){ //This conflictSet is used in repairSolution and improveSolution function

    //Builds a conflict set for the bid
    struct BidsSet *head=bids;
    while(head){
        if(bid==(head->bid)){
            head=head->link;
            continue;
        }
        //int flag=checkForConflict(head->bid,bid);
        //printf("\n\n%d\n\n",flag);
        if(checkForConflict_usingBinary(head->bid,bid)){
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

void displaySolution(struct Solution *solution){
	printf("\nTotal Cost: %lf",solution->totalCost);
	printf("\nNo.of Bids in the solution: %d\n",solution->nBids);
	int i;
	struct BidsSet *bids=solution->bids;
	while(bids){
		struct Bid *bid=bids->bid;
		displayBid(bid);
		bids=bids->link;
	}
}

void displayConflictSet(struct conflictSet *root){// for debug purpose
    struct conflictSet *bids=root;
	while(bids){
		struct Bid *bid=bids->bid;
		displayBid(bid);
		bids=bids->link;
	}
}

void shuffle(int *u){ // Used to generate different random solutions
	int i;
	for(i=0;i<no_of_bids;++i){
		int j=(int)(uol*no_of_bids);
		int temp=u[i];
		u[i]=u[j];
		u[j]=temp;
	}
}

void addBid(struct BidsSet **root,struct Bid *bid){ //adding Bid without checking for conflict
	struct BidsSet *temp=createBidNode(bid);
	if(*root==NULL)
		*root=temp;
	else{
		temp->link=(*root);
		*root=temp;
	}
}

void copySolution(struct Solution *new_sol,struct Solution *solution){ // copys the present solution and creates a new solution. Used as we need both E'(new solution) and E (old solution) to compare.

    struct BidsSet *bids=solution->bids;
    new_sol->nBids=solution->nBids;
    new_sol->totalCost=solution->totalCost;
    struct BidsSet *new_sol_bids=new_sol->bids;
    while(bids){
        addBid(&new_sol->bids,bids->bid);
        bids=bids->link;
    }

}

struct Solution* unionSolution(struct Solution *solution){// Used it earlier. Not using now
    /*
        *Modify union,repair and improve function such that they create a new solution instead of modifing the existing solution.
        *CopySolution() - to get a new solution I am thinking to copy all the contents of solution to new solution and apply union to the new one.
    */
	struct Solution *new_sol=(struct Solution*)malloc(sizeof(struct Solution));
	struct BidsSet *bids=best_sol->bids;
	new_sol->bids=NULL;
	new_sol->nBids=0;
	new_sol->totalCost=0;
	//struct BidsSet *headBid=solution->bids;
    copySolution(new_sol,solution);
	while(bids){
		struct BidsSet *temp=new_sol->bids;
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

void repairSolution(struct Solution *new_sol){// Repair Operator

    //struct Solution *new_sol=(struct Solution*)malloc(sizeof(struct Solution));
    //copySolution(new_sol,solution); //creates a new solution by copying the old solution as we need both later to compare.
    while(1){// only breaks when there are no bids with conflict
        struct BidsSet *head=new_sol->bids;
        int n=new_sol->nBids;
        int *cnt=(int*)malloc(n*sizeof(int));
        float *cost=(float*)malloc(n*sizeof(float));
        memset(cnt,0,n*sizeof(int));
        int i=0;
        while(head){// for getting conflict count for each bid
            cost[i]=head->bid->price;
            struct conflictSet *conflicts=conflictBids[head->bid->sl_no];
            struct BidsSet *temp=new_sol->bids;
            while(temp){
                struct conflictSet* tconflicts=conflicts;
                while(tconflicts){
                    if(temp->bid==tconflicts->bid){
                        ++cnt[i];// counter of conflict
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
        /*
            todo make sure while iterating this is not doing the same function again and again.We have to just remove a bid
        */
        //following is roulette selection of a bid to remove
        double *roulettespace=(double*)malloc(n*sizeof(double));
        double *probabilty=(double*)malloc(n*sizeof(double));
        double rouletteTotal=0;
        for(i=0;i<n;++i){
            probabilty[i]=cnt[i]/cost[i];
            rouletteTotal+=probabilty[i];
            if(probabilty[i]>max){
                j=i;
                max=probabilty[i];
            }
        }
        if(max==0)//breaks when no bid with conflicts
            break;
        roulettespace[0]=probabilty[0]/rouletteTotal;
        for(i=1;i<n;++i){
            roulettespace[i]=roulettespace[i-1]+(probabilty[i]/rouletteTotal);
        }
        double rouletteSelect=uol;

        for(i=0;i<n;++i){
            if(rouletteSelect<=roulettespace[i]){
                j=i;
                break;
            }
        }
        //printf("\n%d %lf\n",j,max);
        head=new_sol->bids;
        struct BidsSet *prev=NULL;
        //traves to the selected bid using j.
        while(j--){
            prev=head;
            head=head->link;
        }
        //removes the selected bid.
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
        //free(cnt);
        //free(cost);
    }
    //printf("\n%d\n",new_sol->nBids);
}

void repairSolution1(struct Solution *new_sol){// modifying Repair Operator
    //looks not correct
    //not completed. Taking more time than the previous one
    //while(1){// only breaks when there are no bids with conflict
    //struct Solution *new_sol=(struct Solution*)malloc(sizeof(struct Solution));
    //copySolution(new_sol,solution);
        struct BidsSet *head=new_sol->bids;
        int n=new_sol->nBids;
        //int *cnt=(int*)malloc(n*sizeof(int));
        struct ListCntCost *cntCostList=NULL;
        struct ListCntCost *tail=NULL;
        //float *cost=(float*)malloc(n*sizeof(float));
        //memset(cnt,0,n*sizeof(int));
        int i;
        //for(i=0;i<n;++i){
        //    tail=insertCntCostatTail(&cntCostList,tail,0,0.0);
        //}
        i=0;
        /*

            while(head){// for getting conflict count for each bid
                cost[i]=head->bid->price;
                struct conflictSet *conflicts=conflictBids[head->bid->sl_no];
                struct BidsSet *temp=new_sol->bids;
                while(temp){
                    struct conflictSet* tconflicts=conflicts;
                    while(tconflicts){
                        if(temp->bid==tconflicts->bid){
                            ++cnt[i];// counter of conflict
                        }
                        tconflicts=tconflicts->link;
                    }
                    temp=temp->link;
                }
                head=head->link;
                ++i;
            }

        */
        while(head){// for getting conflict count for each bid
            //cost[i]=head->bid->price;
            struct conflictSet *conflicts=conflictBids[head->bid->sl_no];
            struct BidsSet *temp=new_sol->bids;
            int tempCnt=0;
            while(temp){
                struct conflictSet* tconflicts=conflicts;
                while(tconflicts){
                    if(temp->bid==tconflicts->bid){
                       // ++cnt[i];// counter of conflict
                       ++tempCnt;
                    }
                    tconflicts=tconflicts->link;
                }
                temp=temp->link;
            }
            tail=insertCntCostatTail(&cntCostList,tail,tempCnt,head->bid->price);
            head=head->link;
            //++i;
        }

    //}

    /*while(thead){
        printf("%d  %f\n",thead->cnt,thead->cost);
        thead=thead->link;
    }*/

    while(1){
        //printf("\n\n\n");

        double max=0;
        int j=-1;
        double *roulettespace=(double*)malloc(n*sizeof(double));
        double *probabilty=(double*)malloc(n*sizeof(double));
        double rouletteTotal=0;
        //head=new_sol->bids;
        struct ListCntCost *thead=cntCostList;
        int N=0;
        while(thead){
            probabilty[N]=thead->cnt/thead->cost;
            rouletteTotal+=probabilty[N];
            if(probabilty[N]>max){
                j=N;
                max=probabilty[N];
            }
            thead=thead->link;
            ++N;
        }

       // printf("\nN:%d\t, n:%d\n",N,new_sol->nBids);
        if(max==0)//breaks when no bid with conflicts
            break;
        roulettespace[0]=probabilty[0]/rouletteTotal;
        for(i=1;i<N;++i){
            roulettespace[i]=roulettespace[i-1]+(probabilty[i]/rouletteTotal);
        }
        double rouletteSelect=uol;
        //for(i=0;i<N;++i){
        //    printf("%lf\n",roulettespace[i]);
        //}

        for(i=0;i<N;++i){
            if(rouletteSelect<=roulettespace[i]){
                j=i;
                break;
            }
        }
        head=new_sol->bids;
        thead=cntCostList;
        struct BidsSet *prev=NULL;
        struct ListCntCost *tprev=NULL;
        while(j--){
            prev=head;
            tprev=thead;
            head=head->link;
            thead=thead->link;
        }
        struct BidsSet *temp=head;
        if(head==new_sol->bids){
            new_sol->bids=head->link;
            cntCostList=thead->link;
            --(new_sol->nBids);
            new_sol->totalCost-=head->bid->price;
        }
        else{
            prev->link=head->link;
            tprev->link=thead->link;
            --(new_sol->nBids);
            new_sol->totalCost-=head->bid->price;
        }
        head=new_sol->bids;
        thead=cntCostList;
        while(head){
            if(checkForConflict_usingBinary(head->bid,temp->bid)){

                --(thead->cnt);
            }
            thead=thead->link;
            head=head->link;
        }
        free(roulettespace);
        free(probabilty);
    }
    //printf("\n%d\n",new_sol->nBids);

}

void improveSolution(struct Solution *solution){// improve operator

	while(1){
        int i,j;
        struct Set *U=NULL;
        struct BidsSet *bids=bidList;
        int Ubid_count=0,add_count=0;
        float max=0;
        // the following Constructs U, set of bids that dont conflict with the solution
        while(bids){
            int flag=1;
            struct BidsSet *head=solution->bids;
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
                ++Ubid_count;// no.of bids in U
            }
            bids=bids->link;
        }

        int *cu=(int *)calloc(Ubid_count,sizeof(int));//counter variable cui
        //memset(cu,0,Ubid_count*sizeof(int));
        float *cost=(float*)malloc(Ubid_count*sizeof(float));
        struct Set *tempU=U;
        i=0;

        while(tempU){
            cost[i]=tempU->bid->price;
            struct conflictSet *conflicts=conflictBids[tempU->bid->sl_no];
            struct Set *temp=U;
            while(temp){
                struct conflictSet* tconflicts=conflicts;
                while(tconflicts){
                    if(temp->bid==tconflicts->bid){
                        ++cu[i];
                    }
                    tconflicts=tconflicts->link;
                }
                temp=temp->link;
            }
            if(cu[i]==0){ //adding solutions directly if no conflict with other bids
                addBid(&(solution->bids),tempU->bid);
                ++(solution->nBids);
                solution->totalCost+=tempU->bid->price;
                ++add_count;
            }
            tempU=tempU->link;
            ++i;
        }
        if(Ubid_count-add_count==0){ // breaks if nothing to add
            break;
        }

        //following is roulette selection of a bid to add
        double *roulettespace=(double*)malloc(Ubid_count*sizeof(double));
        double *probabilty=(double*)malloc(Ubid_count*sizeof(double));
        double rouletteTotal=0;
        for(i=0;i<Ubid_count;++i){
            if(cu[i]==0)
                continue;
            probabilty[i]=cost[i]/cu[i];
            if(probabilty[i]>max){
                j=i;
                max=probabilty[i];
            }
        }
        roulettespace[0]=probabilty[0]/rouletteTotal;
        for(i=1;i<Ubid_count;++i){
            roulettespace[i]=roulettespace[i-1]+(probabilty[i]/rouletteTotal);
        }
        double rouletteSelect=uol;
        for(i=0;i<Ubid_count;++i){
            if(rouletteSelect<=roulettespace[i]){
                j=i;
                break;
            }
        }
        while(j--){ //traves to the selected bid
            U=U->link;
        }
        //adding the bid to solution
        addBid(&(solution->bids),U->bid);
        ++(solution->nBids);
        solution->totalCost+=U->bid->price;

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

void improveSolution1(struct Solution *solution){// modifying this improve operator
        // not complete

    while(1){
        int i,j;
        struct Set *U=NULL;
        struct BidsSet *bids=bidList;
        int Ubid_count=0,add_count=0;
        float max=0;
        // the following Constructs U, set of bids that dont conflict with the solution
        while(bids){
            int flag=1;
            struct BidsSet *head=solution->bids;
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
                ++Ubid_count;// no.of bids in U
            }
            bids=bids->link;
        }

        int *cu=(int *)malloc(Ubid_count*sizeof(int));//counter variable cui
        memset(cu,0,Ubid_count*sizeof(int));
        float *cost=(float*)malloc(Ubid_count*sizeof(float));
        struct Set *tempU=U;
        i=0;

        while(tempU){
            cost[i]=tempU->bid->price;
            struct conflictSet *conflicts=conflictBids[tempU->bid->sl_no];
            struct Set *temp=U;
            while(temp){
                struct conflictSet* tconflicts=conflicts;
                while(tconflicts){
                    if(temp->bid==tconflicts->bid){
                        ++cu[i];
                    }
                    tconflicts=tconflicts->link;
                }
                temp=temp->link;
            }
            if(cu[i]==0){ //adding solutions directly if no conflict with other bids
                addBid(&(solution->bids),tempU->bid);
                ++(solution->nBids);
                solution->totalCost+=tempU->bid->price;
                ++add_count;
            }
            tempU=tempU->link;
            ++i;
        }
        if(Ubid_count-add_count==0){ // breaks if nothing to add
            break;
        }

        //following is roulette selection of a bid to add
         double *roulettespace=(double*)malloc(Ubid_count*sizeof(double));
        double *probabilty=(double*)malloc(Ubid_count*sizeof(double));
        double rouletteTotal=0;
        for(i=0;i<Ubid_count;++i){
            if(cu[i]==0)
                continue;
            probabilty[i]=cost[i]/cu[i];
            if(probabilty[i]>max){
                j=i;
                max=probabilty[i];
            }
        }
        roulettespace[0]=probabilty[0]/rouletteTotal;
        for(i=1;i<Ubid_count;++i){
            roulettespace[i]=roulettespace[i-1]+(probabilty[i]/rouletteTotal);
        }
        double rouletteSelect=uol;
        for(i=0;i<Ubid_count;++i){
            if(rouletteSelect<=roulettespace[i]){
                j=i;
                break;
            }
        }
        while(j--){ //traves to the selected bid
            U=U->link;
        }
        //adding the bid to solution
        addBid(&(solution->bids),U->bid);
        ++(solution->nBids);
        solution->totalCost+=U->bid->price;
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

struct Solution* generate_random_solution(){ // generates a single random solution
    int i;
    int *u=(int *)malloc(no_of_bids*sizeof(int));
    for(i=0;i<no_of_bids;++i)
		u[i]=i;
    struct Solution *solution=(struct Solution*)malloc(sizeof(struct Solution));
    solution->bids=NULL;
    solution->nBids=0;
    solution->totalCost=0;
    shuffle(u); // to randomize the selection of bids
    for(i=0;i<no_of_bids;++i){
        if(insertBid(&solution->bids,&Bids[u[i]])){
            solution->totalCost+=Bids[u[i]].price;
            ++(solution->nBids);

        }

    }
    return solution;
}

struct Solution** generate_random_solutions(int ne){ // generates ne random solutions
    int i,j;
    struct Solution **solutions=(struct Solution**)malloc(ne*sizeof(struct Solution*));

	int *u=(int *)malloc(no_of_bids*sizeof(int));
	for(i=0;i<no_of_bids;++i){
		u[i]=i;
	}

	for(i=0;i<ne;++i){
        solutions[i]=(struct Solution*)malloc(sizeof(struct Solution));
		solutions[i]->bids=NULL;
		solutions[i]->nBids=0;
		solutions[i]->totalCost=0;
		shuffle(u);
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

struct Solution* find_best_sol(struct Solution *solutions,int ne){ //finding the best solution from all solutions
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

struct Solution* generate_Neighbour_Solution(struct Solution *solution){ // generating neighbour solution

    //struct Solution *new_sol1=unionSolution(solution);
    struct Solution *new_sol=(struct Solution*)malloc(sizeof(struct Solution));
    new_sol->bids=NULL;
    new_sol->nBids=0;
    new_sol->totalCost=0;
    struct BidsSet *sol_bids=solution->bids;
    //following adds bids to the new solution by selecting bids from old solution with some probability
    while(sol_bids){
        if(uol<=probabilty_take_from_solution){ // changing this makes the program slow don't know why
            addBid(&(new_sol->bids),sol_bids->bid);
            ++(new_sol->nBids);
            new_sol->totalCost+=sol_bids->bid->price;
        }

        sol_bids=sol_bids->link;
    }

    struct BidsSet *diff_bids_newsol_best=NULL;
    //struct BidsSet *diff_bids_newsol_best=(struct BidsSet*)malloc(sizeof(struct BidsSet));
    //diff_bids_newsol_best=NULL;
    struct BidsSet *best_bids=best_sol->bids;
    //construct the list of bids that are not in the new solution
    while(best_bids){
        struct BidsSet *temp_newsol_bids=new_sol->bids;
        int flag=0;
        while(temp_newsol_bids){
            if(temp_newsol_bids->bid==best_bids->bid){
                flag=1;
                break;
            }
            temp_newsol_bids=temp_newsol_bids->link;
        }
        if(flag==0){
            addBid(&diff_bids_newsol_best,best_bids->bid);
            //printf("price:%lf\n",diff_bids_newsol_best->bid->price);
        }

        best_bids=best_bids->link;
    }
    //following adds resulting bids from above to the solution with some
    while(diff_bids_newsol_best){
        if(uol<=probabilty_take_from_best){ // changing this makes the program slow don't know why
            addBid(&(new_sol->bids),diff_bids_newsol_best->bid);
            ++(new_sol->nBids);
            new_sol->totalCost+=diff_bids_newsol_best->bid->price;

        }
        //diff_bids_newsol_best->bid=NULL;
        diff_bids_newsol_best=diff_bids_newsol_best->link;
    }
    if(REPAIR)
        repairSolution1(new_sol);//check if correct repair function is called
    else
        repairSolution(new_sol);
    /**/
    improveSolution(new_sol);//L2_1000_256_2.txt is very slow with this function. Without this its fast

    /*
        * need to add improveSolution() also
    */
    //tdiff=NULL;
    //free(tdiff);
    return new_sol;
}

int Select_and_Return_Index(int ne){ // Used in ABC for onlookers

    //return an index between 0 to ne-1;
    return (int)(uol*ne);

}

void ABC(){ //ABC algorithm
    int i;
    /*
        * Stoping condition can be in different ways.
        * stoping_count : no.of iterations.
        * change of best_sol: if certain no.of iterations are completed without any change in best_sol.
        * time.
    */
    int stoping_count=ABC_ITERATIONS; //no.of iteration to be made


	int ne=NE,no=NO;// employee bees and onlooker bees
	int noimp=LIMIT_OF_ITERATIONS_WITH_NOCHANGE;//no.of iterations without any update of the solution.
	int *changeCount=(int*)calloc(ne,sizeof(int));
	//memset(changeCount,0,ne*sizeof(int));
    struct Solution **solutions=generate_random_solutions(ne);// generates random solution
    best_sol=find_best_sol(*solutions,ne); // finds best solution
    while(stoping_count--){
        for(i=0;i<ne;++i){// for employee bees
            struct Solution *new_sol=generate_Neighbour_Solution(solutions[i]);
            //printf("\n%d\n",new_sol->nBids);
            if(new_sol->nBids==0){// if new solution has no bids then assign a random solution
                //struct Solution *temp=solutions[i];
                solutions[i]=generate_random_solution();
                changeCount[i]=0;
                //temp->bids=NULL;
                //free(temp);
            }
            else if(new_sol->totalCost>solutions[i]->totalCost){// if new solution is greater than old. assign the new solution
                //struct Solution *temp=solutions[i];
                solutions[i]=new_sol;
                changeCount[i]=0;

            }
            else if(changeCount[i]>=noimp){// if no change in the solution for noimp iterations the assign random solution
                //struct Solution *temp=solutions[i];
                solutions[i]=generate_random_solution();
                changeCount[i]=0;
                //temp->bids=NULL;
                //free(temp);
            }
            else{
                ++changeCount[i];// increments if no change
            }

            if(solutions[i]->totalCost>best_sol->totalCost) //if the solution is better than the best then assign it to best_sol
                best_sol=solutions[i];
        }
        int *p=(int *)malloc(no*sizeof(int));
        struct Solution **S=(struct Solution**)malloc(no*sizeof(struct Solution*));
        for(i=0;i<no;++i){ // for onlooker bees
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
            //Epi is solutions[p[i]]
            p[i]=Select_and_Return_Index(ne); //random number in [0,ne-1].
            S[i]=generate_Neighbour_Solution(solutions[p[i]]);
            if(S[i]->nBids==0){
                //artificially assign fitness worst than Epi to S[i]
                //S
                S[i]->totalCost=-1.0;
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
    free(solutions);
}

int main(){
    time_t start,end,gstart;
	int i,j;
	/*
        Code to skip unwanted lines in input
	*/
	int bytes_read;
    size_t nbytes = 100;
    char *my_string;

    my_string = (char *) malloc (nbytes + 1);

    char str[200];
	bytes_read = getline (&my_string, &nbytes, stdin);
	while(my_string[0]=='%'||my_string[0]=='\n'){ //skips the starting parts. i.e not useful input
		getline (&my_string, &nbytes, stdin);
	}

	/*
        "goods #num" is already in my_string so we have to get no_of_goods from my_string
	*/
	while(my_string[0]!=' '){
        ++my_string;
	}++my_string;
    no_of_goods=get_num_from_string(my_string);// getting no_of_goods


	//srand( time(NULL));
    srand(SEED);
    /*
        get goods,bids,dummy from input
    */
	//scanf("%s %d",my_string,&no_of_goods); //
	scanf("%s %d",my_string,&no_of_bids);
	scanf("%s %d",my_string,&no_of_dummy);


	Bids=(struct Bid*)malloc(no_of_bids*sizeof(struct Bid));// Bids memory is allocated here
	conflictBids=(struct conflictSet**)malloc(no_of_bids*sizeof(struct conflictSet*));

	for(i=0;i<no_of_bids;++i){

		scanf("%d",&Bids[i].sl_no);
		scanf("%lf",&Bids[i].price);
		Bids[i].goods=NULL;
		Bids[i].goods_binary=(int*)calloc(no_of_goods+no_of_dummy,sizeof(int));
		//memset(Bids[i].goods_binary,0,(no_of_goods+no_of_dummy)*sizeof(int));
		char good;
		int count=0,space=0;

		scanf("%c",&good);// we are taking each character as we have to know when # occurs

		while(good!='#'){ // each bid ends with '#'. so we are iterating untill we get encounter it
			int flag=0,res=0;
			if(good==' ')
				space++;
			while(isdigit(good)){
				flag=1; //to know if we have to add to bid
				res=res*10+good-'0'; //for more than one digit number
				scanf("%c",&good);
			}

			if(flag){
                    ++(Bids[i].goods_binary[res]);
                    insertGood(&Bids[i].goods,res);
                    ++count;
			}
			scanf("%c",&good);
		}
		Bids[i].n=count; //no.of goods in that bid.
	}

	for(i=0;i<no_of_bids;++i){ // creating the list of bids used for improve operator
        struct BidsSet *temp=createBidNode(&Bids[i]);
        if(bidList==NULL){
            bidList=temp;
        }
        else{
            temp->link=bidList;
            bidList=temp;
        }
	}


    for(i=0;i<no_of_bids;++i){ // creating conlict set for each bid. Used for repair and improve operator
        constructConflictSet(&conflictBids[i],&Bids[i],bidList);
        //printf("\n\n********Conflict Set for Bid: %d",i);
        //displayConflictSet(conflictBids[i]);
    }
    printf("Using REPAIR:%d,  Prob_take_sol:%lf, Prob_take_best:%lf\n",REPAIR,probabilty_take_from_solution,probabilty_take_from_best);
    printf("\nGoods :%d\n",no_of_goods);
	printf("Bids: %d\n",no_of_bids);
	printf("Dummy: %d\n",no_of_dummy);

    gstart=time(NULL);
    for(i=0;i<ITERATE;++i){
        srand(SEED+i);
        start=time(NULL);// using this I am getting time in seconds
        ABC();
        end=time(NULL);
        //start-end gives 0 if it executes in less than a second.
        if((gbest_sol==NULL)||(gbest_sol->totalCost<best_sol->totalCost)){
            gbest_sol=best_sol;
        }
        printf("\n\n%d  Best:%lf\nTime taken: %lf seconds\n",i,best_sol->totalCost,difftime(end,start));

        int *gbinary=get_binary_goods_for_Solution(best_sol);
        for(j=0;j<no_of_goods+no_of_dummy;++j){
            printf("%d ",gbinary[j]);
        }
        printf("\n\n");
        //displaySolution(best_sol);
    }
    end=time(NULL);


    printf("\n\nGlobal Best:%lf\nTotal Time taken:%lf\n",gbest_sol->totalCost,difftime(end,gstart));
    free(Bids);
    free(conflictBids);
    free(best_sol);
    free(bidList);


}
