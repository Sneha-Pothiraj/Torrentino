#include<semaphore.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>
#include<unistd.h>
#include<ctime>
#include<fstream>
using namespace std;

string str_arr[100][100];
int arr[100][100];
int pointer[100];
int count = 0;
sem_t update_mut;
sem_t print_mut;

int print(int status,int *user_borrower,int user_lender,int file_num)
{
    sem_wait(&print_mut);
    string str = "file"+to_string(file_num)+".txt";
    fstream fp_local;
    string temp="";
    fp_local.open(str,ios::in);
    if(!fp_local)
    {
        perror("Error: ");
        return(-1);
    }
    else 
    {
        cout<<"\n\n"<<str<<" is currently downloading..."; 
        getline(fp_local,temp);
        str_arr[*user_borrower][file_num-1] = temp;
    }
    fp_local.close();
    if(status==1)
    {
        pointer[file_num-1] = (*user_borrower)+1;
        cout<<"\nUser "<<(*user_borrower+1)<<" has downloaded the file "<<file_num<<" from User "<<user_lender<<"\n";
        arr[*user_borrower][file_num-1] = 1;
    }
    if(status==0)
    {
        pointer[file_num-1] = (*user_borrower)+1;
        cout<<"\nUser "<<(*user_borrower+1)<<" has downloaded the file "<<file_num<<" from the global directory\n";
        arr[*user_borrower][file_num-1] = 1;
    }
    sem_post(&print_mut);
    return 0;
}

void *file_download(void *arg)
{
    int status = 0;
    int *i = (int *)arg;
    int counter = 0;
    
    while (counter<count)
    {
        int random_num = 0;
        random_num = (rand()%count)+1;
    
        while(1)
        {
            int s = 0;
            
            if(arr[*i][random_num-1]==1)
            {
                random_num = (rand()%count)+1;
                s = 1;
            }
            
            if(s==1)
                continue;
            else
                break;
        }
        
        sem_wait(&update_mut);
        
        if(pointer[random_num-1]==0)
        {
            status = 0;
            print(status,i,0,random_num);
        }
        
        else
        {
            status = 1;
            print(status,i,pointer[random_num-1],random_num);
        }
        
        sem_post(&update_mut);
        counter++;
    }
}

int main()
{
    sem_init(&update_mut,0,1);
    sem_init(&print_mut,0,1);
    fstream fp_global;
    string line = "";
    time_t t;
    srand((unsigned)time(&t));
    fp_global.open("lines.txt",ios::in);
    
    if(!fp_global)
    {
        perror("Error: ");
        return(-1);
    }
    
    while(getline(fp_global,line))
        count++;
    
    fp_global.close();
    fp_global.open("lines.txt",ios::in);
    
    if(!fp_global)
    {
        perror("Error: ");
        return(-1);
    }

    for(int i=1;i<=count;i++)
    {
        fstream fp_local;
        string str = "file"+to_string(i)+".txt";
        string temp = "";
        fp_local.open(str,ios::out);
        if(fp_local.is_open())
        {
            getline(fp_global,temp);
            fp_local<<temp;
        }

        fp_local.close();

    }

    fp_global.close();
    pthread_t files[count];
    
    for(int i=0;i<count;i++)
        for(int j=0;j<count;j++)
            arr[i][j] = 0;
    
    for(int i=0;i<count;i++)
        pointer[i] = 0;
    
    int k[100];
    
    for(int i=0;i<count;i++)
    {
        k[i] = i;
       pthread_create(&files[i],NULL,file_download,(void*)&k[i]);
    }

    for (int i = 0; i<count; ++i)
    {
		pthread_join(files[i],NULL);
    }

    cout<<"\n\n";

    for(int i=0;i<count;i++)
    {
        for(int j=0;j<count;j++)
            cout<<" "<<str_arr[i][j];
        cout<<"\n";
    }

    cout<<"\n\n";
    return 0;
}   


