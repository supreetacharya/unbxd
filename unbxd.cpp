//============================================================================
// Name        : unbxd.cpp
// Author      : Suprit
// Version     :
// Copyright   : Your copyright notice
// Description : Unbxd assignment in C++, Ansi-style
//============================================================================

#include <iostream>

#include <string>
#include <fstream>
#include <vector>
#include <cstring>
using namespace std;


/*
 Product class basically stores the product details
 */
class Product{
public:
	string genre;
	string name;
	string id;
	string artist;
	string pid;
	string str;
	//Product();
	Product(string n,string g,string i,string a){
		genre=g;
		id=i;
		name=n;
		artist=a;
		pid=i;
		str=pid+" "+name+" "+genre+" "+artist;
	}
};


/*
 Query class stores the query details
*/
class Query{
public:
	string id;
	string query;
	string pid;
	string str;
	string tstamp;
	Query(string i,string t,string p){
		query=i;
		id=i;
		pid=p;
		tstamp=t;
		str=query+" "+pid;

	}
};

/*
 Generic leaf node which comprises a singly linked list and stores the object in the sorted order

 */
template <class T>
class leaf{
public:
	vector<T> records;// records to be stored
	leaf *next;
	int lmax;//
	leaf(int lsize,T object){
		lmax=lsize;next=NULL;records.push_back(object);
	}

	/*
	It returns the exact position where insertion needs to take place
	 */
	int bsearch(string key,int low,int high){
		while(low<=high){
			if(records[low].id >= key)
				return low;
			int mid=(low+high)/2;
			if(records[mid].id < key)
				low=mid+1;
			else high=mid;
			}
		return low;
	}
	/*
	 * The add function inserts the record into the leaf
	 */
	leaf* add(T elem,string &key){
		leaf *current=this;
		while(current->next!=NULL && current->records[0].id==current->next->records[0].id)
			current=current->next;
		if(current->records.size() < lmax){
			int pos=bsearch(elem.id,0,current->records.size()-1);
			current->records.insert(current->records.begin()+pos,1,elem);
			key=current->records[0].id;
			return NULL;

		}
		else{
			int pos=bsearch(elem.id,0,current->records.size()-1);
			current->records.insert(current->records.begin()+pos,1,elem);
			int mid=(0+lmax)/2;
			string midkey=current->records[mid].id;
			if(midkey!=current->records[0].id){// oth key and mid key are different new parent
			vector<T> temp1(current->records.begin(),current->records.begin()+mid-1);
			leaf *sibling = new leaf<T>(lmax,current->records[mid]);
			sibling->records.insert(sibling->records.begin()+1,current->records.begin()+mid+1,current->records.end());
			current->records.clear();current->records=temp1;
			key=sibling->records[0].id;
			sibling->next=current->next;
			current->next=sibling;
			return sibling;
			}
			else{
				if(midkey==current->records[lmax].id){
					leaf *sibling = new leaf<T>(lmax,current->records[lmax]);
					current->records.pop_back();
					sibling->next=current->next;
					current->next=sibling;
					key=midkey;
					return NULL;
				}
				else{
					int i=mid;
					while(midkey==current->records[i].id)
						i++;
					leaf *sibling=new leaf<T>(lmax,current->records[i]);
					key=sibling->records[0].id;
					vector<T> temp(current->records.begin()+i+1,current->records.end());
					current->records.erase(current->records.begin()+i,current->records.end());
					sibling->records.insert(sibling->records.begin()+1,temp.begin(),temp.end());
					sibling->next=current->next;
					current->next=sibling;
					return sibling;
				}
			}


		}

	}

	/*
	 The search function is used to search the required record ids.
	 */

	vector<T> search(string key){
		leaf *current=this;
		vector<T> tmp;
		int pos;
		while(current!=NULL){
			pos=current->bsearch(key,0,current->records.size()-1);
			if(pos==current->records.size())
				return tmp;

			for(int i=pos;i<current->records.size();i++){
				if(current->records[i].id > key)
					return tmp;
				else if(current->records[i].id==key){
					tmp.push_back(current->records[i]);
				//	cout<<current->records[i].str<<endl;
				}
			}
			current=current->next;
		}
	}




};

/*
 * The internal nodes of the bPlustree are nonleaf nodes.
 * It can point to the leaf nodes or the non leaf nodes.
 *If the index[i]="key" then nlnodes[i] will store the pointers to the nonleaf nodes greater than
 *the key and nlnodes[i-1] will store the pointers to the nonleaf nodes less than the key.
 *If isleaf=1, which means that the nonleaf points to the leaf nodes.
 *In the above case lnodes[i] will store the pointers to the leaf nodes with
 *keys greater or equal to the "key" and lnodes[i-1] store the pinters to the nodes smaller
 *than "key";
 */
template <class T>
class nonleaf{
public:
	vector<string> index;//stores the keys
	vector<nonleaf*> nlnodes;//pointers to non leaf nodes
	vector<leaf<T>*> lnodes;// pointes to the leaf nodes
	int nlmax,lmax,isleaf;// isleaf denotes whether the leaf nodes point to a leaf node or non leaf node. isleaf=1 means that it points to the leaf node.
	nonleaf(int lsize,int nlsize){
		nlmax=nlsize;lmax=lsize;
		lnodes.push_back(NULL);
		nlnodes.push_back(NULL);
		index.push_back("");

	}
	int bsearch(string key,int low,int high){
		if(key < index[low])
			return 0;
		while(low<=high){
			if(index[low]>=key){
				if(index[low]==key)
				return low;
				else
					return low-1;
			}

			int mid=(low+high)/2;
			if(index[mid]<key)
				low=mid+1;
			else
				high=mid;

		}
		if(low==index.size())
			low=low-1;
		return low;
	}

	nonleaf *add(string val,string &keyval,T object,int isroot){
		int pos=bsearch(val,1,index.size()-1);

		if(isleaf){
			if(lnodes[pos]==NULL){//oth position insertion
				leaf<T> *tmpnode=new leaf<T>(lmax,object);
				tmpnode->next=lnodes[pos+1];
				lnodes[pos]=tmpnode;
				keyval=index[pos+1];
				return NULL;
			}
			else if(lnodes[pos]!=NULL){
				string key="";
				leaf<T> *tmpnode=lnodes[pos]->add(object,key);
				if(tmpnode==NULL) return NULL;
			//	int pos1=bsearch(key,1,index.size()-1);
			//	if(pos1==0)pos1=1;
				int newpos=pos+1;
				index.insert(index.begin()+newpos,1,key);
				lnodes.insert(lnodes.begin()+newpos,1,tmpnode);
				if(index.size() <= nlmax+1)
					return NULL;
				int mid=(index.size())/2;
				keyval=index[mid];
				nonleaf *sibling=new nonleaf<T>(lmax,nlmax);
				sibling->isleaf=1;
				sibling->index.insert(sibling->index.begin()+1,index.begin()+mid+1,index.end());
				sibling->lnodes.pop_back();
				sibling->lnodes.insert(sibling->lnodes.begin(),lnodes.begin()+mid,lnodes.end());
				index.erase(index.begin()+mid,index.end());
				lnodes.erase(lnodes.begin()+mid,lnodes.end());
				if(isroot){
					nonleaf *parent=new nonleaf<T>(lmax,nlmax);
					parent->isleaf=0;
					parent->index.insert(parent->index.begin()+1,1,keyval);
					parent->nlnodes[0]=this;
					parent->nlnodes.push_back(sibling);
					return parent;
				}
				return sibling;
			}
		}
		else{//non leaf
			if(nlnodes[pos]!=NULL){
				string key="";
				nonleaf<T> *tmpnode=nlnodes[pos]->add(val,key,object,0);
				if(tmpnode==NULL) return NULL;
				index.insert(index.begin()+pos+1,1,key);
				nlnodes.insert(nlnodes.begin()+pos+1,1,tmpnode);
				if(index.size() <= nlmax+1)
					return NULL;
				int mid=(index.size())/2;
				keyval=index[mid];
				nonleaf *sibling = new nonleaf<T>(lmax,nlmax);
				sibling->isleaf=0;
				sibling->index.insert(sibling->index.begin()+1,index.begin()+mid+1,index.end());
				sibling->nlnodes.pop_back();
				sibling->nlnodes.insert(sibling->nlnodes.begin(),nlnodes.begin()+mid,nlnodes.end());
				index.erase(index.begin()+mid,index.end());
				nlnodes.erase(nlnodes.begin()+mid,nlnodes.end());
				if(isroot){
					nonleaf *parent = new nonleaf<T>(lmax,nlmax);
					parent->isleaf=0;
					parent->index.insert(parent->index.begin()+1,1,keyval);
					parent->nlnodes.pop_back();
					parent->nlnodes.push_back(this);
					parent->nlnodes.push_back(sibling);
					return parent;
				}
				return sibling;
			}
			return NULL;
		}
	}

	vector<T> search(string key){
		int pos=bsearch(key,1,index.size()-1);
		if(isleaf){
			return lnodes[pos]->search(key);
		}
		else
			return nlnodes[pos]->search(key);
	}



};

/*
 * Generic bplustree implementation that stores the root to the non leaf nodes.
 */
template <class T>
class bPlusTree{
public:
	nonleaf<T> *root;
	int lmax,nlmax;
	bPlusTree(int lsize,int nlsize){
		lmax=lsize;
		nlmax=nlsize;
	}
	void insert(string val,T object){
		if(root==NULL){
			root=new nonleaf<T>(lmax,nlmax);
			root->isleaf=1;
			root->index.push_back(val);
			leaf<T> *temp=new leaf<T>(lmax,object);
			root->lnodes.push_back(temp);
		}
		else{
			string key="";
			nonleaf<T> *tmpnode=root->add(val,key,object,1);
			if(tmpnode!=NULL)
				root=tmpnode;
		}
	}
	vector<T> search(string key){

		if(root!=NULL)
		return root->search(key);

	}
};

/*
 * This function returns the Product object given a product string.
 */
Product newgetProductTokens(char*str){

	////
	string field="",value="";
	char *l=str;
	vector<string> tmp;
	for(int i=0,stp=0,sti=1,st=1;i<strlen(l);i++){
		if(sti){

			if(st && (l[i]==' ' || l[i]=='\t'))
				continue;
			if(st && l[i]=='\"' && l[i+1]!=':'){
				st=0;
				continue;
			}
			if(l[i]=='\"' && l[i+1]==':'){
				i++;
				value="";
				sti=0;stp=1;st=1;
				continue;
			}
			field+=l[i];
		}
		else if(stp){

			if(st && (l[i]==' ' || l[i]=='\t'))
				continue;

			if(st && l[i]=='\"' && l[i+1]!=','){
				st=0;
				continue;
			}
			if(st && field=="productId"){
				st=0;
			}

			if(l[i]=='\"' && l[i+1]==','){
				i++;
				tmp.push_back(value);
				field="";
				sti=1;stp=0;st=1;
				continue;
			}
			value+=l[i];






		}


	}
	if(value!="")
		tmp.push_back(value);

	if(tmp.size()<4){
		tmp.clear();
		return Product("","","","");
	}
	Product P(tmp[1],tmp[0],tmp[3],tmp[2]);
	tmp.clear();
	return P;




	/////


}


/*
 * This function parses the product file and build the bplusTrees , one with product id as index
 * and the other with artist as the index. The leaf nodes stores the product information in
 * both the bplus trees.
 */
void buildProductIndex(bPlusTree<Product> *&productTreeProductIndex,bPlusTree<Product> *&productTreeArtistIndex,char *productFile=""){
	ifstream is;
	//	is.open("/Users/supreet/Documents/data/data_sample.txt",ifstream::in);
	is.open(productFile,ifstream::in);
	string str="";
		int read=0;
		int count=0;
		while(is.good()){
			char c=is.get();
			if(is.good()){
				switch (c){
				case '{':
					read=1;
					str="";
					continue;
				case '[':
					continue;
				case '}':
				{
					read=0;

					Product t=newgetProductTokens((char*)str.c_str());
					t.str=str;
					if(t.id.size()==0)
						break;

					productTreeProductIndex->insert(t.id,t);
					t.id=t.artist;
					productTreeArtistIndex->insert(t.id,t);


					count++;
					break;
				}
				case ']':
					break;
				case '\n':
					break;
				}
				if(read && c!='{')
				str+=c;
			}
		}
		is.close();
		//cout<<"Product count "<<count<<endl;
}

/*
 * This function returns the Query object when provided with the query string.
 */

Query getnewQueryTokens(char *str){

	string field="",value="";
		char *l=str;
		vector<string> tmp;
		for(int i=0,stp=0,sti=1,st=1;i<strlen(l);i++){
			if(sti){

				if(st && (l[i]==' ' || l[i]=='\t'))
					continue;
				if(st && l[i]=='\"' && l[i+1]!=':'){
					st=0;
					continue;
				}
				if(l[i]=='\"' && l[i+1]==':'){
					i++;
					value="";
					sti=0;stp=1;st=1;
					continue;
				}
				field+=l[i];
			}
			else if(stp){

				if(st && (l[i]==' ' || l[i]=='\t'))
					continue;

				if(st && l[i]=='\"' && l[i+1]!=','){
					st=0;
					continue;
				}
				if(st && field=="productId"){
					st=0;
				}

				if(l[i]=='\"' && l[i+1]==','){
					i++;
					tmp.push_back(value);
					field="";
					sti=1;stp=0;st=1;
					continue;
				}
				value+=l[i];






			}


		}
		if(value!="")
			tmp.push_back(value);

		if(tmp.size()<3){
			tmp.clear();
			return Query("","","");
		}
		Query Q(tmp[0],tmp[1],tmp[2]);
		tmp.clear();
		return Q;

}

/*
 * This function parses the query file and builds the bplustrees where index of one the tree is query string
 * and and that of other is product id.
 * It stores a query record in the leaf nodes.
 */
void buildQueryIndex(bPlusTree<Query> *&queryQueryIndex,bPlusTree<Query> *&queryProductIdIndex,char *file=""){
	ifstream is;
	//	is.open("/Users/supreet/Documents/data/query_sample.txt",ifstream::in);
	is.open(file,ifstream::in);
	string str="";
		int read=0;
		int count=0;
		while(is.good()){
			char c=is.get();
			if(is.good()){
				switch (c){
				case '{':
				{
					read=1;
					str="";
					break;
				}
				case '[':
					break;
				case '}':
				{
					read=0;


				Query t = getnewQueryTokens((char*)str.c_str());
				t.str=str;
				if(t.id.size()==0)
					break;

				queryQueryIndex->insert(t.id,t);
				t.id=t.pid;
				queryProductIdIndex->insert(t.id,t);

					count++;
					break;
				}
				case ']':
					break;
				case '\n':
					break;
				default:
					break;
				}
				if(read && c!='{')
				str+=c;
			}
		}

	//	cout<<"Query Count "<<count<<endl;
}

/*
 * This function searched the matching queries for a given artist.
 */
void searchArtist(string artist,bPlusTree<Product> *&productArtistIndex,bPlusTree<Query> *&queryProductIdIndex){

	vector<Product> t=productArtistIndex->search(artist);
	if(t.size()==0){
		cout<<"No Such Artist Exists"<<endl;

	}
	for(int i=0;i<t.size();i++){
		cout<<"The product ID is "<<t[i].pid<<endl;
		vector<Query> tmp = queryProductIdIndex->search(t[i].pid);
		if(tmp.size()==0)
			cout<<"No Records Found"<<endl;

		for(int j=0;j<tmp.size();j++)
			cout<<"query = "<<tmp[i].str<<endl;

	}



}

/*
 * This function returns all the product information for a particular query.
 */
void searchQuery(string query,bPlusTree<Query> *&queryQueryIndex,bPlusTree<Product> *&productProductIndex){

	vector<Query> t=queryQueryIndex->search(query);
	if(t.size()==0)
		cout<<"No Such Query Exists"<<endl;
	for(int i=0;i<t.size();i++){
		cout<<"The Product Id is "<<t[i].pid<<endl;
		vector<Product> tmp = productProductIndex->search(t[i].pid);
		if(tmp.size()==0)
			cout<<"No Records Found"<<endl;
		for(int j=0;j<tmp.size();j++)
			cout<<"Product = "<<tmp[i].str<<endl;
	}
}

int main() {




	bPlusTree<Product> *productProductIndex,*productArtistIndex;
	bPlusTree<Query> *queryQueryIndex,*queryProductIdIndex;
	productProductIndex=new bPlusTree<Product>(10000,100);
	productArtistIndex=new bPlusTree<Product>(10000,100);
	queryQueryIndex=new bPlusTree<Query>(10000,100);
	queryProductIdIndex=new bPlusTree<Query>(10000,100);

	string productFile,queryFile;
	cout<<"Enter the Product File Path"<<endl;
	cin>>productFile;
	cout<<"Wait while the correct product indexes are created"<<endl;
	buildProductIndex(productProductIndex,productArtistIndex,(char*)productFile.c_str());
	cout<<"Product Indexes have been created"<<endl;
	cout<<"Enter the Query File Path"<<endl;
	cin>>queryFile;
	cout<<"Wait while the correct indexes are created"<<endl;
	buildQueryIndex(queryQueryIndex,queryProductIdIndex,(char*)queryFile.c_str());
	cout<<"Query indexes are created"<<endl;

	cout<<"Options for interactive Console"<<endl;
	cout<<"(A) Enter the Artist Name"<<endl;
	cout<<"(Q) Enter the Query Name"<<endl;
	cout<<"(q) To quit the console"<<endl;
	string option="";
	string artist="";
		string query="";
	cout<<"Enter the Option Type>";
	char c;
	while(option!="q"){

		option="";
		cout<<endl;

		do{
			c=getchar();
			if(c=='\n')
				break;
			option+=c;

		}while(c!='\n');


		if(option=="A")
		{

			cout<<"Enter the ArtistName>"<<endl;
			artist="";

			do{
				c=getchar();
				if(c=='\n')
					break;
				artist+=c;
			}while(c!='\n');



			searchArtist(artist,productArtistIndex,queryProductIdIndex);
			cout<<endl;
			cout<<"Enter the Option Type>";

		}
		else if(option=="Q")
		{

			query="";
			cout<<"Enter the QueryName>"<<endl;
			do{
				c=getchar();
				if(c=='\n')
					break;
				query+=c;
			}while(c!='\n');


			searchQuery(query,queryQueryIndex,productProductIndex);
			cout<<endl;
			cout<<"Enter the Option Type>";
		}
		else if(option=="q"){
			break;

		}

	}


	return 0;
}
