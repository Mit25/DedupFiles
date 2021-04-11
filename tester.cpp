#include<bits/stdc++.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<signal.h>
#include <sys/wait.h>
#include<setjmp.h>
#include<fcntl.h>
#include<pthread.h>
#include <sys/stat.h>
#include <semaphore.h>
#include"global.h"
#include"md5.h"
using namespace std;
typedef long long ll;

sem_t lock;
map<string,vector<string> > m;
map<string,vector<string> >::iterator it;
ll filecount=0,digestcount=0;
double tottime=0;

void *func(void *data){
	char *ptmp=(char *)data;
	string path(ptmp);
	printf("Opened Directory: %s\n",ptmp);
	int p[2];
	if(pipe(p)<0)
		return (void *)-1;
	if(fork()==0){
		dup2(p[1],1);
		printf("not\n");
		execl("/bin/ls","bin/ls",ptmp,NULL);
		exit(0);
	}
	else{
		wait(NULL);
		char inbuf[1000000];
		int len=read(p[0],inbuf,sizeof(inbuf));
		vector<string> v;
		char *pch=strtok(inbuf,"\t\n");
		pch=strtok(NULL,"\t\n");
		while(pch!=NULL){
			string tmp(pch);
			v.push_back(tmp);
			pch=strtok(NULL,"\t\n");
		}

		//vector<pthread_t> vtid;
		for(int i=0;i<v.size();i++){
			printf("%s", v[i].c_str());
			struct stat sb;
			string allpath=path+"/"+v[i];
			char aptmp[allpath.length()+1];
			strcpy(aptmp,allpath.c_str());
			if (stat(aptmp, &sb) == 0 && S_ISDIR(sb.st_mode)){
			    printf(": Directory\n");
			    //func(aptmp);
			    pthread_t tid;
			    pthread_create(&tid, NULL, func, (void *)aptmp);
			    pthread_join(tid,NULL);
			    //vtid.push_back(tid);
			}
			else{
				printf(": File\n");
				char hash[33];
				clock_t t;
    			t = clock();    
				MDFile(aptmp,hash);
			    t = clock() - t;
			    double time_taken = ((double)t)/CLOCKS_PER_SEC;
			    tottime+=time_taken;
				string digest(hash);
				sem_wait(&lock);
				filecount++;
				if((it=m.find(digest))==m.end()){
					vector<string> vec;
					vec.push_back(aptmp);
					m.insert(pair<string,vector<string> >(digest,vec));
				}
				else{
					it->second.push_back(aptmp);
				}
				sem_post(&lock);
			}
		}
		//for(int i=0;i<vtid.size();i++)
		//	pthread_join(vtid[i],NULL);
	}
	return 0;
}

int main(){
	char ptmp[1024];string path;
	//getcwd(ptmp, sizeof(ptmp));
	printf("Enter a path:");
	scanf("%s",ptmp);
	sem_init(&lock, 0, 1);
	cout<<"Starting duplicate file search in this directory..."<<endl;
	clock_t t;
	t = clock();    
	pthread_t tid;
	pthread_create(&tid, NULL, func, (void *)ptmp);
	pthread_join(tid, NULL);
	//func(ptmp);
    t = clock() - t;
    double runtime = ((double)t)/CLOCKS_PER_SEC;
	for(it=m.begin();it!=m.end();it++){
		cout<<it->first<<": ";
		digestcount++;
		for(int i=0;i<it->second.size();i++){
			if(i!=0)
				cout<<"\n                                  ";
			cout<<it->second[i];
		}cout<<endl;
	}
	cout<<"\nStatistics:\n***********\nTotal no. of files: "<<filecount<<"\nNo. of duplicate files: "<<filecount-digestcount<<"\nAverage time for calculating 128 bit message digest: "<<tottime/filecount<<"s\nTotal running time: "<<runtime<<"s"<<endl;
	return 0;
}