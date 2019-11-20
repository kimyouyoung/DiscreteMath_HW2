#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node{
      int type; // 0 - simple 1 - complex 
      int value; //if simple, it represents # after a

      //0 - and 1- or 2- not
      int complex; //if complex, it represents logical term. 
      struct node** values; //multiple values are linked through the nodes
      int num;
}node;

/*
      int type; // 0 - simple 1 - complex 
      int value; //if simple, it represents # after a
      //0 - and 1- or 2- not
      int complex; //if complex, it represents logical term. 
*/
node* createNode(int type, int value, int complex){
      node *a = (node*)malloc(sizeof(node));
      a->type=type;
      a->value = value;
      a->complex = complex;
      a->num = 0;
      return a;
}

node* copyNode(node* src){
      node* dest = (node*)malloc(sizeof(node));
      dest->type=src->type;
      dest->value = src->value;
      dest->complex = src->complex;
      dest->num = src->num;
      dest->values = (node**)malloc(sizeof(node*));
      for(int i =0; i<src->num;i++){
            dest->values[i] = copyNode(src->values[i]);
      }
      return dest;
}

/*
full delete-> mod = 0
else mod any
*/
void freeNode(node** root,int mod){
      node* r = *root;
      if(mod==0) free(r->values);//full delete; if you reused(copied) the node, don't erase it
      free(r);
}
void printNodeDebug(node* root,int indent){
      for(int i= 0;i<indent;i++){
            printf("  ");
      }
      if(root->type==0){
            printf("a%d ", root->value);
      }
      else {
            if(root->complex == 0) printf("and ");
            else if(root->complex == 1) printf("or ");
            else printf("not ");
      }
      printf("%d \n",root->num);
      
      if(root->type!=0)for(int i =0; i<root->num;i++){
            printNodeDebug(root->values[i],indent +1);
      }
}
void printNode(node* root){
      if(root->type==0) printf("%d",root->value);
      else {
            for(int i=0;i<root->num;i++){
                  if(root->complex==2) printf("-");
                  printNode(root->values[i]);
                  switch(root->complex){
                        case 0: printf(" ");
                        break;
                        case 1: printf("\n");
                        break;
                  } 
            }
      }
}

int satisfiable(node* root, int* num, int* a, int n, int* value, int* r){
      if(root->type==0) {
            num[*a] = (root->value) * n;
            *a = *a +1;
      }else {
            for(int i=0;i<root->num;i++){
                  if(root->complex==2)
                        satisfiable(root->values[i], num, a, -1, value, r);
                  else
                        satisfiable(root->values[i], num, a, 1, value, r);
                  switch(root->complex){
                        case 0: 
                              *value = (*value) * num[i];
                              break;
                        case 1: 
                              if((*value) > 0)
                                    *r = 1;
                              break;
                  } 
            }
      }
      if((*r) > 0) return 0;
      return -1;
}


void trimP(char *temp){
      char buf[16];
      strcpy(buf,temp);
      strcpy(temp,"\0");
      int j = 0;
      for(int i =0;i<16;i++){
            if(buf[i]=='\0') temp[j] = '\0';
            if(buf[i] != '('){
                  temp[j] = buf[i];
                  j++;
            }
      }
}

char** count(const char temp[128], int* a){
      int start = 0, end = 0, j = 0, sz = 4;
      int flag = 0;
      char** str = (char**)malloc(sizeof(char*)*sz);

      for(int i =0; i<128;i++){
            char buf[128] ="";
            switch(temp[i]){
                  case '(': //block start
                        flag++;
                        break;
                  case ')': //block end
                        flag--;
                        break;
                  case ' ': //new block
                  case '\0':
                        if(flag==0){
                              end = i;
                              if(start !=end){
                                    strncpy(buf,temp+start,end - start);
                                    // printf("%s\n", buf);
                                    if(j>sz-1){//if assigned size of strings are too small, it will double its capacity.
                                          sz*=2;
                                          str = (char**)realloc(str,sizeof(char*)*sz);
                                    }
                                    str[j] = (char*)malloc(sizeof(char)*64);
                                    str[j][0] = '\0';
                                    strcpy(str[j],buf);
                                    // printf("%d: %s\n",j,str[j]);
                                    j++;
                                    strcpy(buf,"");
                              }
                              start = end+1;
                        }
                        break;
                  default:
                        break;
            }
            if(temp[i]=='\0')break;
      }
      *a=j;
      return str;
}



node* parse(char temp[128]){
      // printf("%s\n",temp);
      char buf[16] = "";
      int intro;
      node* a= (node*)malloc(sizeof(node));
      
      a->type = 0; 
      a->value = -1;
      a->complex = -1;
      a->num =0;

      sscanf(temp,"%s",buf);
      intro = strlen(buf);
      if(buf[0] =='('){
            trimP(buf);
            temp[(int)strlen(temp)-1] = '\0';
      }

      if(strcmp(buf,"and") == 0){
            a->type = 1;
            a->complex = 0;
      } else if(strcmp(buf,"or")==0){
            a->type = 1;
            a->complex = 1;
      } else if(strcmp(buf,"not")==0){
            a->type = 1;
            a->complex = 2;
      } else {
            a->type = 0;
            a->value = atoi(buf+1);
      }
      if(a->type == 1){
            char** str = count(temp +intro +1,&a->num);
            a->values = (node**)malloc(sizeof(node*)*a->num);
            for(int i =0; i<a->num;i++){
                  a->values[i] = parse(str[i]);
            }
            //free str
            int num = 4;
            while(num<a->num){
                  num*=2;
            }
            for(int i =0; i<num;i++){
                  free(str[i]);
            }
            free(str);
      }
      return a;
}

void nnf(node* root){
      node* child;
      for(int i =0; i<root->num;i++){
            child = root->values[i];
            if(child->complex!=2) continue;
            //not node has only 1 child node;
            node* grandchild = child->values[0];
            //if grandchild is a leaf don't touch
            if(grandchild->type==0) continue;
            //gc == not
            if(grandchild->complex == 2){
                  free(child);
                  root->values[i] = grandchild->values[0];
                  free(grandchild);
            }
            //and or
            else {
                  free(child);
                  root->values[i] = grandchild;
                  grandchild->complex = !grandchild->complex;
                  node* ggc;
                  for(int i =0; i<grandchild->num;i++){
                        ggc = grandchild->values[i];
                        //make not node directing ggc
                        node *a = (node*)malloc(sizeof(node));
                        a->type=1;
                        a->value = -1;
                        a->complex = 2;

                        a->num = 1;
                        a->values = (node**)malloc(sizeof(node*));
                        a->values[0] = ggc;
                        //reconnect grandchild ->a
                        grandchild->values[i] = a;
                  }
            }
      }
      for(int i =0; i<root->num;i++){
            nnf(root->values[i]);
      }
}

node* dnf(node* root){
      if(root->type ==0) return root;
      node* child;
      int flag = 0;
      for(int i =root->num -1; i>=0;i--){
            child = root->values[i];
            //commutative law
            if(root->complex == child->complex){
                  int n = root->num;
                  root->num += child->num -1;
                  root->values = (node**)realloc(root->values, sizeof(node*)*root->num);
                  root->values[i] = child->values[0];
                  for(int j = n,k=1;j<root->num;j++,k++){
                        root->values[j] = child->values[k];
                  }
                  flag = 1;
            }
            //distributive law
            else if(root->complex==0&&child->complex==1){
                  node* child2;
                  int idx = i;
                  if(i==0&&root->num==1){
                        freeNode(&root,0);
                        root = child;
                        flag = 1;
                        break;
                  }
                  if(i==root->num -1){ //마지막 노드 일때
                       child2 = root->values[i-1];
                       idx =i-1;
                  }
                  else { //inbetween
                        child2 = root->values[root->num-1];
                  }

                  //and node
                  node *a = createNode(1,-1,1);//or
                  a->num = child->num;
                  a->values = (node**)malloc(sizeof(node*)*a->num);
                  //or node as child
                  for(int i =0; i<a->num;i++){
                        a->values[i] = createNode(1, -1, 0); //and
                        a->values[i]->num = 2;
                        a->values[i]->values = (node**)malloc(sizeof(node*)*2);
                        a->values[i]->values[0] = child->values[i];
                        a->values[i]->values[1] = copyNode(child2);
                  }
                  freeNode(&child2,0);
                  freeNode(&child,0);
                  root->values[idx] = a;
                  root->num--;
                  root->values = (node**)realloc(root->values,sizeof(node*)*root->num);
            }
      }
      // printNodeDebug(root,0);
      // getchar();
      if(flag==1) root = dnf(root);
      for(int i =0; i<root->num;i++){
            root->values[i] = dnf(root->values[i]);
      }
      // root = dnf(root);
      return root;
}

int checkDNF(node* root){
      if(root->type == 0) return 1;
      for(int i =0; i<root->num;i++){
            node* child = root->values[i];
            if(child->type ==0) continue;
            if(root->complex==root->values[i]->complex) return 0;
            if(root->complex==0&&root->values[i]->complex==1) return 0;
            if(checkDNF(root->values[i])==0) return 0;
      }
      return 1;
}

int compare(node* a, node* b){
      //simple
      //complex
      if(a->type == b->type){
            if(a->type==0) 
                  return a->value > b->value;
            else
                  return a->num > b->num;
      }
      //simple complex
      if(a->type == 0){
            if (b->complex ==2)
                  return a->value > b->values[0]->value;
            else return 0;
      }
      else{
            if (a->complex ==2)
                  return a->values[0]->value > b->value;
            else return 1;
      }
}

void swap(node** a, node** b){
      node* temp = *a;
      *a = *b;
      *b = temp;
}

void quickSort(node*** array, int start, int end){
      node** arr = *array;
      if(start<end){
            int mid = (start+end)/2;
            swap(&arr[start],&arr[mid]);
            mid = start;
            int right= end +1;
            for(int i =mid+1; i<right;i++){
                  if(compare(arr[mid],arr[i])==0){
                        swap(&arr[i],&arr[right-1]);
                        right--;
                        i--;
                  }
                  else{
                        swap(&arr[mid],&arr[i]);
                        mid++;
                  }
            }
            quickSort(&arr,start,mid-1);
            quickSort(&arr,mid+1,end);
      }
}

void sortNode(node** root){
      node* a = *root;
      //sort
      quickSort(&a->values,0,a->num-1);
      for(int i =0;i<a->num;i++){
            sortNode(&a->values[i]);
      }
}

/* 0 - Error 1 - NoError*/
int checkNode(node* root){
      if(root->type==1){
            if(root->complex==2){
                  if(root->num!=1) return 0;
            }
            else {
                  if(root->num<1) return 0;
            }
            for(int i =0; i<root->num;i++){
                  checkNode(root->values[i]);
            }
      }
      else if(root->value<0) return 0;
      else return 1;
}

int checkError(char temp[128]){
      int p = 1;
      if(temp[0]!='(') return 0;
      for(int i =1; i<128; i++){
            switch(temp[i]){
                  case 'a': case 'o': case 'r':case 'n': case 't': case 'd': //legal char
                  case '0': case '1': case '2': case '3': case '4':
                  case '5': case '6': case '7': case '8': case '9':
                  case ' ': case '\0':
                        break;
                  case '(':p++; break;
                  case ')':p--; break;
                  default:
                        return 0;
            }
            if(temp[i]=='\0') break;
            if(p<0) return 0;
      }
      if(p!=0) return 0;
      return 1;
}

void terminate(){
      printf(" =================================== \n");
      printf(" DNF CONVERTER: INVALID INPUT \n");
      printf("\n Allowed chars (n for integer): \n");
      printf(" and or not ( ) a1 a2 a3 ... an \n");
      printf("\n Input has to be in following form: \n");
      printf(" EX) (or a1 (not (and a2 a3)))\n");
      printf("Please Fix your input \n");
      printf(" =================================== \n");
      exit(0);
}

int main(){
      //char temp[128]="(or a1 (not (or (not (or a2 a3)) a4)))";
      char temp[128] = "";
      char str[128];
      int flag = 0;

      while(!feof(stdin)){
            scanf("%s", str);
            for(int i=0; i<(int)strlen(str); i++){
                  if(str[i] == '(')
                        flag++;
                  else if(str[i] == ')')
                        flag--;
            }
            strcat(temp, str);
            strcat(temp, " ");
            if(flag == 0) break;
      }

      temp[(int)strlen(temp)-1] = '\0';

      if(checkError(temp)==0) terminate();
      node* root = parse(temp);
      if(checkNode(root)==0) terminate();
      nnf(root);
      // printNodeDebug(root,0);
      while(checkDNF(root)==0){
            root = dnf(root);
      }
      sortNode(&root);
      // printNodeDebug(root,0);
      printNode(root);
      printf("0\n");
      int num[1000];
      int k, l = 0;
      int min, ex;
      int a = 0;
      int n = 1;
      int value = 1;
      int r = -1;
      int sat = satisfiable(root, num, &a, n, &value, &r);
      int fi[a];
      if(sat == -1) 
            printf("UNSAT\n");
      else if(sat >= 0){
            for(int i = 0; i < a; i++){
                  k = 0;
                  for(int j = 0; j < i; j++){
                        if(abs(num[i]) == abs(num[j])){
                              k = 1;
                              break;
                        }
                  }
                  if(k == 0){
                        fi[l] = num[i];
                        l++;
                  }
            }
            
            for (int i = 0; i < l - 1; i++) {
		      min = i;
		      for (int j = i + 1; j < l; j++)
			      if (abs(fi[j]) < abs(fi[min]))
				      min = j;
			      ex = fi[i];
			      fi[i] = fi[min];
			      fi[min] = ex;
                  
            }
	}

            for(int i =0 ; i< l ; i++)
			printf("%d ",fi[i]);
		printf("\n");
      

      return 0;
}