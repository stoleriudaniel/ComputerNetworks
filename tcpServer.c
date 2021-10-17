#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sqlite3.h>

#define PORT 4444

void init_introTxt(char introTxt[250])
{
	//strcpy(introTxt,"\nSalut! Te poti conecta la cont folosind comanda \"autentificare\".\n");
	//strcat(introTxt,"Autentificarea este necesara pentru a avea acces la restul comenzilor.\n");
	//strcat(introTxt,"Daca nu ai un cont, te poti inregistra folosind comanda \"inregistrare\".\n\n");
	strcpy(introTxt,"\n[Server] Bine ati venit in aplicatia TopMusic.\n\n");
}

int main(){

	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", 4444);

	if(listen(sockfd, 10) == 0){
		printf("[+]Listening....\n");
	}else{
		printf("[-]Error in binding.\n");
	}

	char* err;
    sqlite3* db;
    sqlite3_stmt* stmt;
    sqlite3_open("DataBase.db",&db);
	int rc = sqlite3_exec(db,"CREATE TABLE IF NOT EXISTS UtilizatoriDeSistem(numeUtilizator varchar(20), parola varchar(20), admin INT)",NULL,NULL,&err);
    if(rc!=SQLITE_OK)
    {
        printf("eroare: %s",err);
    }
    rc=sqlite3_exec(db,"CREATE TABLE IF NOT EXISTS Melodii(numeMelodie varchar(40), descriere varchar(100), link varchar(120))",NULL,NULL,&err);
    if(rc!=SQLITE_OK)
    {
        printf("eroare: %s",err);
    }
    rc=sqlite3_exec(db,"CREATE TABLE IF NOT EXISTS TopMelodii(numeMelodie varchar(40), voturi INT, genJazz INT, genPop INT, genRap INT, genRock INT, genFolk INT)",NULL,NULL,&err);
	if(rc!=SQLITE_OK)
	{
		printf("eroare: %s",err);
	}
	rc=sqlite3_exec(db,"CREATE TABLE IF NOT EXISTS ComentariiMelodii(numeUtilizator varchar(20), numeMelodie varchar(40), comentariu varchar(100))",NULL,NULL,&err);
    if(rc!=SQLITE_OK)
    {
        printf("eroare: %s",err);
    }
	rc=sqlite3_exec(db,"CREATE TABLE IF NOT EXISTS UtilizatoriRestrictionati(numeUtilizator varchar(20))",NULL,NULL,&err);
    if(rc!=SQLITE_OK)
    {
        printf("eroare: %s",err);
    }
	char introTxt[250];
	init_introTxt(introTxt);
	while(1){
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if((childpid = fork()) == 0){
			close(sockfd);
			int introTxtTrimis=0; int ok=0; int intrebareTrimisa=0; int iesire=0; int uttCorect=-1;
			int inregistrat=0; int utAdmin=0, utObisnuit=0; int autentificat=0; char utilizatorLogat[20]; utilizatorLogat[0]='\0';
			if(!introTxtTrimis){
				send(newSocket,introTxt,strlen(introTxt),0);
				bzero(introTxt,sizeof(introTxt));
				introTxtTrimis=1;
			}
			while(1){
				char comanda[100];
				while(!ok)
				{
					if(!intrebareTrimisa){
						char msg[50]; msg[0]='\0'; bzero(msg,sizeof(msg));
						strcpy(msg,"Aveti deja un cont? [da/nu]\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						intrebareTrimisa=1;
					}
					comanda[0]='\0'; bzero(comanda,sizeof(comanda));
					recv(newSocket,comanda,100,0);
					if(strcmp(comanda,"da")==0)
					{
						//char msg[100]; msg[0]='\0';
						//bzero(msg,sizeof(msg));
						//strcpy(msg,"Doriti sa va autentificati ca utilizator obisnuit sau ca admin? [obisnuit/admin]\n");
						//send(newSocket,msg,strlen(msg),0);
						//bzero(msg,sizeof(msg));
						ok=1; inregistrat=1; intrebareTrimisa=0;
					}
					else if(strcmp(comanda,"nu")==0)
					{
						ok=1;  intrebareTrimisa=0; inregistrat=0;
					}
					else
					{
						char msg[80]; msg[0]='\0';
						strcpy(msg,"Comanda gresita! Incercati din nou.\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
				}
				while(!inregistrat)
				{
					if(!intrebareTrimisa)
					{
						char msg[100]; msg[0]='\0'; bzero(msg,sizeof(msg));
						strcpy(msg,"Doriti sa va intregistrati ca utilizator obisnuit sau ca admin? [obisnuit/admin]\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						intrebareTrimisa=1;
					}
					comanda[0]='\0'; bzero(comanda,sizeof(comanda));
					recv(newSocket,comanda,100,0);
					if(strcmp(comanda,"obisnuit")==0)
					{
						char msg[100]; msg[0]='\0'; bzero(msg,sizeof(msg));
						char numeUtObisnuit[20], parolaUtObisnuit[20];
						numeUtObisnuit[0]='\0'; parolaUtObisnuit[0]='\0';
						strcpy(msg,"Introduceti numele de utilizator:");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(numeUtObisnuit,sizeof(numeUtObisnuit));
						recv(newSocket,numeUtObisnuit,20,0);	
						strcpy(msg,"Introduceti parola:");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(parolaUtObisnuit,sizeof(parolaUtObisnuit));
						recv(newSocket,parolaUtObisnuit,20,0);
						char query[100];
						strcpy(query,"insert into UtilizatoriDeSistem VALUES");
						strcat(query,"(");
						strcat(query,"'");
						strcat(query,numeUtObisnuit);
						strcat(query,"'");
						strcat(query,",");
						strcat(query,"'");
						strcat(query,parolaUtObisnuit);
						strcat(query,"'");
						strcat(query,",");
						strcat(query,"0");
						strcat(query,")");
						int myrc = sqlite3_exec(db,query,NULL,NULL,&err);
						if(myrc!=SQLITE_OK)
						{
							printf("eroare la inregistre(bd): %s",err);
						}
						inregistrat=1; intrebareTrimisa=0;
						sqlite3_prepare_v2(db,"select numeUtilizator, parola, admin from UtilizatoriDeSistem",-1,&stmt,0);
						const unsigned char *nume, *parola; int adminINT;
						sqlite3_step(stmt);
						nume = sqlite3_column_text(stmt,0);
						parola=sqlite3_column_text(stmt,1);
						adminINT=sqlite3_column_int(stmt,2);
						sqlite3_finalize(stmt);
					}
					else if(strcmp(comanda,"admin")==0)
					{ 
						char msg[200]; msg[0]='\0'; bzero(msg,sizeof(msg));
						char numeUtAdmin[20], parolaUtAdmin[20];
						numeUtAdmin[0]='\0'; parolaUtAdmin[0]='\0';
						strcpy(msg,"Introduceti numele de utilizator:");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(numeUtAdmin,sizeof(numeUtAdmin));
						recv(newSocket,numeUtAdmin,20,0);	
						strcpy(msg,"Introduceti parola:");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(parolaUtAdmin,sizeof(parolaUtAdmin));
						recv(newSocket,parolaUtAdmin,20,0);
						char query[100];
						strcpy(query,"insert into UtilizatoriDeSistem VALUES");
						strcat(query,"(");
						strcat(query,"'");
						strcat(query,numeUtAdmin);
						strcat(query,"'");
						strcat(query,",");
						strcat(query,"'");
						strcat(query,parolaUtAdmin);
						strcat(query,"'");
						strcat(query,",");
						strcat(query,"1");
						strcat(query,")");
						int myrc = sqlite3_exec(db,query,NULL,NULL,&err);
						if(myrc!=SQLITE_OK)
						{
							printf("eroare la inregistre(bd): %s",err);
						}
						inregistrat=1; intrebareTrimisa=0;
					}
					else
					{
						char msg[100];
						strcpy(msg,"Comanda gresita! Incercati din nou.\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
				}
				while(!autentificat) // autentificarea
				{
					if(!intrebareTrimisa)
					{
						char msg[100]; msg[0]='\0';
						strcpy(msg,"Doriti sa va autentificati ca utilizator obisnuit sau ca admin? [obisnuit/admin]\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						intrebareTrimisa=1;
					}
					comanda[0]='\0'; bzero(comanda,sizeof(comanda));
					recv(newSocket,comanda,100,0);
					if(strcmp(comanda,"obisnuit")!=0 && strcmp(comanda,"admin")!=0)
					{
						char msg[40];
						strcpy(msg,"Comanda incorecta! Incercati din nou\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
					else if(strcmp(comanda,"obisnuit")==0)
					{
						char msg[300]; utObisnuit=1; int myOk=0;
                       	while(myOk==0)
						{
							char numeUtObisnuit[20], parolaUtObisnuit[20];
							numeUtObisnuit[0]='\0'; parolaUtObisnuit[0]='\0';
							if(uttCorect==0) strcpy(msg,"Cont incorect! Incercati din nou.\nIntroduceti numele de utilizator obisnuit:");
							else strcpy(msg,"Introduceti numele de utilizator obisnuit:");
							send(newSocket,msg,strlen(msg),0);
							bzero(msg,sizeof(msg));
							bzero(numeUtObisnuit,sizeof(numeUtObisnuit));
							recv(newSocket,numeUtObisnuit,20,0);
							strcpy(msg,"Introduceti parola:");
							send(newSocket,msg,strlen(msg),0);
							bzero(msg,sizeof(msg));
							bzero(parolaUtObisnuit,sizeof(parolaUtObisnuit));
							recv(newSocket,parolaUtObisnuit,20,0);
							const unsigned char* numeUt, *parolaUt;
							sqlite3_prepare_v2(db,"select numeUtilizator, parola from UtilizatoriDeSistem where admin=0",-1,&stmt,0);
							while(sqlite3_step(stmt)!=SQLITE_DONE)
							{
								numeUt=sqlite3_column_text(stmt,0);
								parolaUt=sqlite3_column_text(stmt,1);
								char numeUtv[20]; char parolaUtv[20];
								strcpy(numeUtv,numeUt); strcpy(parolaUtv,parolaUt);
								if(strcmp(numeUt,numeUtObisnuit)==0 && strcmp(parolaUt,parolaUtObisnuit)==0)
								{
									uttCorect=1; strcpy(utilizatorLogat,numeUt); myOk=1; autentificat=1;
								}
							}
							sqlite3_finalize(stmt);
							if(uttCorect==-1) uttCorect=0;
						}
						strcat(msg,"V-ati conectat ca utilizator obisnuit.\n");
						strcat(msg,"Aveti la dispozitie urmatoarele comenzi: 'afisareInformatiiMelodie' 'afisareComenzi' 'adaugareMelodie' 'votareMelodie' 'afisareTopGeneral' 'afisareTopDupaGen' 'adaugareComentariuMelodie' 'afisareComentariiMelodie' 'iesire'\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
					else if(strcmp(comanda,"admin")==0)
					{
						char msg[300]; utObisnuit=1; int myOk=0;
                       	while(myOk==0)
						{
							char numeUtAdmin[20], parolaUtAdmin[20];
							numeUtAdmin[0]='\0'; parolaUtAdmin[0]='\0'; strcpy(parolaUtAdmin,""); strcpy(numeUtAdmin,""); 
							if(uttCorect==0) strcpy(msg,"Cont incorect! Incercati din nou.\nIntroduceti numele de utilizator admin:");
							else strcpy(msg,"Introduceti numele de utilizator admin:");
							send(newSocket,msg,strlen(msg),0);
							bzero(msg,sizeof(msg));
							bzero(numeUtAdmin,sizeof(numeUtAdmin));
							recv(newSocket,numeUtAdmin,20,0); 
							bzero(parolaUtAdmin,sizeof(parolaUtAdmin));
							strcpy(msg,"Introduceti parola:");
							send(newSocket,msg,strlen(msg),0);
							bzero(msg,sizeof(msg));
							recv(newSocket,parolaUtAdmin,20,0);
							const unsigned char* numeUt, *parolaUt;
							sqlite3_prepare_v2(db,"select numeUtilizator, parola from UtilizatoriDeSistem where admin=1",-1,&stmt,0);
							while(sqlite3_step(stmt)!=SQLITE_DONE)
							{
								numeUt=sqlite3_column_text(stmt,0);
								parolaUt=sqlite3_column_text(stmt,1);
								char numeUtv[20]; char parolaUtv[20]; numeUtv[0]='\0'; parolaUtv[0]='\0';
								strcpy(numeUtv,numeUt); strcpy(parolaUtv,parolaUt);
								if(strcmp(numeUt,numeUtAdmin)==0 && strcmp(parolaUt,parolaUtAdmin)==0)
								{
									uttCorect=1; utAdmin=1; strcpy(utilizatorLogat,numeUt);
									myOk=1; autentificat=1;
								}
							}
							sqlite3_finalize(stmt);
							if(uttCorect==-1) uttCorect=0;
						}
						strcat(msg,"V-ati conectat ca utilizator admin.\n");
						strcat(msg,"Aveti la dispozitie urmatoarele comenzi: 'afisareInformatiiMelodie' 'afisareComenzi' 'adaugareMelodie' 'votareMelodie' 'afisareTopGeneral' 'afisareTopDupaGen' 'adaugareComentariuMelodie' 'afisareComentariiMelodie' 'stergereMelodie' 'restrictieVotUtilizator' 'inlaturareRestrictie' 'iesire'\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
				}
				while(1) // autentificat
				{
					comanda[0]='\0'; bzero(comanda,sizeof(comanda));
					recv(newSocket,comanda,100,0); 
					if(strcmp(comanda,"afisareComenzi")==0 && !utAdmin)
					{
						char msg[300]; bzero(msg,sizeof(msg));
						strcpy(msg,"Aveti la dispozitie urmatoarele comenzi: 'afisareInformatiiMelodie' 'afisareComenzi' 'adaugareMelodie' 'votareMelodie' 'afisareTopGeneral' 'afisareTopDupaGen' 'adaugareComentariuMelodie' 'afisareComentariiMelodie' 'iesire'\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
					else if(strcmp(comanda,"afisareComenzi")==0 && utAdmin)
					{
						char msg[300]; bzero(msg,sizeof(msg));
						strcat(msg,"Aveti la dispozitie urmatoarele comenzi: 'afisareInformatiiMelodie' 'afisareComenzi' 'adaugareMelodie' 'votareMelodie' 'afisareTopGeneral' 'afisareTopDupaGen' 'adaugareComentariuMelodie' 'afisareComentariiMelodie' 'stergereMelodie' 'restrictieVotUtilizator' 'inlaturareRestrictie' 'iesire'\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
					else if(strcmp(comanda,"adaugareMelodie")==0)
					{
						char msg[400];
						char numeMelodie[40]; numeMelodie[0]='\0';
						char descriereMelodie[100]; descriereMelodie[0]='\0';
						char linkMelodie[120]; linkMelodie[0]='\0';
						char genuri[100]; genuri[0]='\0';
						strcpy(msg,"Introdu numele melodiei:\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(numeMelodie,sizeof(numeMelodie));
						recv(newSocket,numeMelodie,40,0);
						strcpy(msg,"Dati o descriere melodiei:\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(descriereMelodie,sizeof(descriereMelodie));
						recv(newSocket,descriereMelodie,100,0);
						strcpy(msg,"Introduceti genurile melodiei(Jazz,Pop,Rap,Rock,Folk):\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(genuri,sizeof(genuri));
						recv(newSocket,genuri,100,0);
						char *p; int esteJazz=0, estePop=0, esteRap=0, esteRock=0, esteFolk=0;
						p=strtok(genuri,",");
						while(p)
						{
							if(strcmp(p,"Jazz")==0) esteJazz=1;
							if(strcmp(p,"Pop")==0) estePop=1;
							if(strcmp(p,"Rap")==0) esteRap=1;
							if(strcmp(p,"Rock")==0) esteRock=1;
							if(strcmp(p,"Folk")==0) esteFolk=1;
							p=strtok(NULL,",");
						}
						strcpy(msg,"Introduceti linkul catre melodie:\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(linkMelodie,sizeof(linkMelodie));
						recv(newSocket,linkMelodie,120,0);
						char melodieNoua[200];
						strcpy(melodieNoua,"INSERT INTO Melodii VALUES");
						strcat(melodieNoua,"(");
						strcat(melodieNoua,"'");
						strcat(melodieNoua,numeMelodie);
						strcat(melodieNoua,"'");
						strcat(melodieNoua,",");
						strcat(melodieNoua,"'");
						strcat(melodieNoua,descriereMelodie);
						strcat(melodieNoua,"'");
						strcat(melodieNoua,",");
						strcat(melodieNoua,"'");
						strcat(melodieNoua,linkMelodie);
						strcat(melodieNoua,"'");
						strcat(melodieNoua,")");
						rc = sqlite3_exec(db,melodieNoua,NULL,NULL,&err);
						if(rc!=SQLITE_OK)
						{
							printf("eroare la adaugarea melodiei[bd]: %s",err);
						}
						//TopMelodii(numeMelodie varchar(20), voturi INT, genJazz INT, genPop INT, genRap INT, genRock INT, genFolk INT
						strcpy(melodieNoua,"INSERT INTO TopMelodii VALUES");
						strcat(melodieNoua,"(");
						strcat(melodieNoua,"'");
						strcat(melodieNoua,numeMelodie);
						strcat(melodieNoua,"'");
						strcat(melodieNoua,",");
						strcat(melodieNoua,"0");
						strcat(melodieNoua,",");
						strcat(melodieNoua,((esteJazz)? "1" : "0"));
						strcat(melodieNoua,",");
						strcat(melodieNoua,((estePop)? "1" : "0"));
						strcat(melodieNoua,",");
						strcat(melodieNoua,((esteRap)? "1" : "0"));
						strcat(melodieNoua,",");
						strcat(melodieNoua,((esteRock)? "1" : "0"));
						strcat(melodieNoua,",");
						strcat(melodieNoua,((esteFolk)? "1" : "0"));
						strcat(melodieNoua,")");
						rc = sqlite3_exec(db,melodieNoua,NULL,NULL,&err);
						if(rc!=SQLITE_OK)
						{
							printf("eroare la adaugarea melodiei[bd]: %s",err);
						}
						strcpy(msg,"MelodieAdaugata.\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
					else if(strcmp(comanda,"votareMelodie")==0)
					{
						char msg[100]; char numeMelodie[40]; bzero(numeMelodie,sizeof(numeMelodie)); bzero(msg,sizeof(msg));
						int restrictionat=0;
						char numeUt[20];
						sqlite3_prepare_v2(db,"select numeUtilizator from UtilizatoriRestrictionati",-1,&stmt,0);
						while(sqlite3_step(stmt)!=SQLITE_DONE)
						{
							const unsigned char* nume; nume=sqlite3_column_text(stmt,0);
							strcpy(numeUt,nume);
							if(strcmp(numeUt,utilizatorLogat)==0){ restrictionat=1;}
						}
						sqlite3_finalize(stmt);
						if(restrictionat)
						{
							bzero(msg,sizeof(msg));
							strcpy(msg,"Nu aveti dreptul de a vota melodii");
							send(newSocket,msg,strlen(msg),0);
							bzero(msg,sizeof(msg));
						}
						if(!restrictionat)
						{
							strcpy(msg,"Ce melodie doriti sa votati?\n\n");
							send(newSocket,msg,strlen(msg),0);
							bzero(msg,sizeof(msg));
							int melodieCorecta=0;
							bzero(numeMelodie,sizeof(numeMelodie));
							recv(newSocket,numeMelodie,40,0);
							while(!melodieCorecta)
							{
								sqlite3_prepare_v2(db,"select numeMelodie from TopMelodii",-1,&stmt,0);
								while(sqlite3_step(stmt)!=SQLITE_DONE)
								{
									const unsigned char* nume; nume=sqlite3_column_text(stmt,0);
									char numeM[40]; strcpy(numeM,nume);
									if(strcmp(numeM,numeMelodie)==0)
									{
										melodieCorecta=1;
									}
								}
								sqlite3_finalize(stmt);
								if(!melodieCorecta)
								{
									bzero(msg,sizeof(msg));
									strcpy(msg,"Melodie inexistenta! Incercati din nou.\n\n");
									send(newSocket,msg,strlen(msg),0);
									bzero(msg,sizeof(msg));
									bzero(numeMelodie,sizeof(numeMelodie));
									recv(newSocket,numeMelodie,40,0);
								}
							}
							sqlite3_stmt *myStmt;
							char *interogare = "UPDATE TopMelodii SET voturi=voturi+1 WHERE numeMelodie= ?";
							//char updateNrVoturiMelodie[100];
							//strcpy(updateNrVoturiMelodie,"UPDATE TopMelodii SET voturi = 1 WHERE numeMelodie = ?");
							//strcat(interogare,"'");
							//strcat(interogare,numeMelodie);
							//strcat(interogare,"'");
							//if(sqlite3_prepare_v2(db,"UPDATE TopMelodii SET voturi = ? WHERE numeMelodie = ?;",-1,&stmt,NULL)!=SQLITE_OK)
							if(sqlite3_prepare_v2(db,interogare,-1,&myStmt,NULL)!=SQLITE_OK)
							{
								printf("eroare[bd]");
							}
							sqlite3_bind_text(myStmt,1,numeMelodie,-1,NULL);
							//sqlite3_bind_int(stmt,1,1);
							sqlite3_step(myStmt);
							sqlite3_finalize(stmt);
							bzero(msg,sizeof(msg));
							strcpy(msg,"Melodia a fost votata.\n\n");
							send(newSocket,msg,strlen(msg),0);
							bzero(msg,sizeof(msg));
						}
						/*else{
							bzero(msg,sizeof(msg));
							strcpy(msg,"Nu aveti dreptul de a vota melodii.\n\n");
							send(newSocket,msg,strlen(msg),0);
							bzero(msg,sizeof(msg));
						}*/
					}
					else if(strcmp(comanda,"afisareTopGeneral")==0)
					{
						char msg[1024]; msg[0]='\0'; int rank=0;
						strcpy(msg,"Topul general al melodiilor este:\n\n");
						const unsigned char* nume; int voturi; 
						sqlite3_prepare_v2(db,"select numeMelodie, voturi from TopMelodii order by voturi desc",-1,&stmt,0);
						while(sqlite3_step(stmt)!=SQLITE_DONE)
						{
							nume=sqlite3_column_text(stmt,0);
							voturi=sqlite3_column_int(stmt,1);
							char strNrVoturi[10]; 
							int inv=0; int c=voturi; 
							while(voturi)
							{
								inv=inv*10+voturi%10;
								voturi/=10;
							}
							int poz=0;
							if(!inv) strNrVoturi[0]='0';
							while(inv)
							{
								if(inv%10==0) strNrVoturi[poz]='0';
								if(inv%10==1) strNrVoturi[poz]='1';
								if(inv%10==2) strNrVoturi[poz]='2';
								if(inv%10==3) strNrVoturi[poz]='3';
								if(inv%10==4) strNrVoturi[poz]='4';
								if(inv%10==5) strNrVoturi[poz]='5';
								if(inv%10==6) strNrVoturi[poz]='6';
								if(inv%10==7) strNrVoturi[poz]='7';
								if(inv%10==8) strNrVoturi[poz]='8';
								if(inv%10==9) strNrVoturi[poz]='9';
								poz++; inv/=10;
							}
							strNrVoturi[poz]='\0'; poz++;
							rank++; int r=rank; char strNrRank[10]; int pr=0;
							inv=0; 
							while(r)
							{
								inv=inv*10+r%10;
								r/=10;
							} 
							while(inv)
							{
								if(inv%10==0) strNrRank[pr]='0';
								if(inv%10==1) strNrRank[pr]='1';
								if(inv%10==2) strNrRank[pr]='2';
								if(inv%10==3) strNrRank[pr]='3';
								if(inv%10==4) strNrRank[pr]='4';
								if(inv%10==5) strNrRank[pr]='5';
								if(inv%10==6) strNrRank[pr]='6';
								if(inv%10==7) strNrRank[pr]='7';
								if(inv%10==8) strNrRank[pr]='8';
								if(inv%10==9) strNrRank[pr]='9';
								pr++; inv/=10;
							}
							strNrRank[pr]='\0'; pr++;
							strcat(msg,strNrRank); 
							strcat(msg,".");
							strcat(msg," ");
							strcat(msg,nume); strcat(msg," "); 
							if(!c) strcat(msg,"0"); else strcat(msg,strNrVoturi);
							strcat(msg," voturi\n");
						}
						sqlite3_finalize(stmt);
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
					else if(strcmp(comanda,"afisareTopDupaGen")==0)
					{
						char msg[1024]; msg[0]='\0'; int rank=0; bzero(msg,sizeof(msg));
						strcpy(msg,"Introduceti genul(Jazz,Pop,Rap,Rock,Folk):\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						char melodie[40]; char Gen[20]; melodie[0]='\0'; bzero(melodie,sizeof(melodie));
						recv(newSocket,melodie,40,0);
						strcpy(Gen,"gen"); strcat(Gen,melodie);
						strcpy(msg,"Topul melodiilor dupa genul dat este:\n\n");
						const unsigned char* nume; int voturi=0;
						char interogare[100];
						strcpy(interogare,"select numeMelodie, voturi from TopMelodii where ");
						strcat(interogare,Gen);
						strcat(interogare,"=1 order by voturi desc");
						sqlite3_prepare_v2(db,interogare,-1,&stmt,0);
						while(sqlite3_step(stmt)!=SQLITE_DONE)
						{
							nume=sqlite3_column_text(stmt,0);
							voturi=sqlite3_column_int(stmt,1);
							char strNrVoturi[10]; strNrVoturi[0]='\0';
							int inv=0;
							while(voturi)
							{
								inv=inv*10+voturi%10;
								voturi/=10;
							}
							int poz=0;
							if(!inv) strcpy(strNrVoturi,"0");
							while(inv)
							{
								if(inv%10==0) strNrVoturi[poz]='0';
								if(inv%10==1) strNrVoturi[poz]='1';
								if(inv%10==2) strNrVoturi[poz]='2';
								if(inv%10==3) strNrVoturi[poz]='3';
								if(inv%10==4) strNrVoturi[poz]='4';
								if(inv%10==5) strNrVoturi[poz]='5';
								if(inv%10==6) strNrVoturi[poz]='6';
								if(inv%10==7) strNrVoturi[poz]='7';
								if(inv%10==8) strNrVoturi[poz]='8';
								if(inv%10==9) strNrVoturi[poz]='9';
								poz++; inv/=10;
								if(!inv){strNrVoturi[poz]='\0'; poz++;}
							}
							rank++; int r=rank; char strNrRank[10]; int pr=0;
							inv=0; 
							while(r)
							{
								inv=inv*10+r%10;
								r/=10;
							} 
							while(inv)
							{
								if(inv%10==0) strNrRank[pr]='0';
								if(inv%10==1) strNrRank[pr]='1';
								if(inv%10==2) strNrRank[pr]='2';
								if(inv%10==3) strNrRank[pr]='3';
								if(inv%10==4) strNrRank[pr]='4';
								if(inv%10==5) strNrRank[pr]='5';
								if(inv%10==6) strNrRank[pr]='6';
								if(inv%10==7) strNrRank[pr]='7';
								if(inv%10==8) strNrRank[pr]='8';
								if(inv%10==9) strNrRank[pr]='9';
								pr++; inv/=10;
							}
							strNrRank[pr]='\0'; pr++;
							strcat(msg,strNrRank);
							strcat(msg,"."); 
							strcat(msg," ");
							strcat(msg,nume); 
							strcat(msg," ");
							strcat(msg,"voturi=");
							strcat(msg,strNrVoturi);
							strcat(msg,"\n"); //strcat(msg,voturi); strcat(msg," voturi\n");
						}
						sqlite3_finalize(stmt);
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
					else if(strcmp(comanda,"adaugareComentariuMelodie")==0)
					{//"CREATE TABLE IF NOT EXISTS ComentariiMelodii(numeUtilizator varchar(20), numeMelodie varchar(40), comentariu varchar(100))"
						char msg[100]; char comm[100]; char melodieM[40];
						strcpy(msg,"Alegeti melodia pe care doriti sa o comentati:\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(melodieM,sizeof(melodieM));
						recv(newSocket,melodieM,100,0);
						strcpy(msg,"Introduceti comentariul:\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(comm,sizeof(comm));
						recv(newSocket,comm,100,0);
						char interogare[400];
						strcpy(interogare,"INSERT INTO ComentariiMelodii VALUES");
						strcat(interogare,"(");
						strcat(interogare,"'");
						strcat(interogare,utilizatorLogat); 
						strcat(interogare,"'");
						strcat(interogare,",");
						strcat(interogare,"'");
						strcat(interogare,melodieM); 
						strcat(interogare,"'");
						strcat(interogare,",");
						strcat(interogare,"'");
						strcat(interogare,comm);
						strcat(interogare,"'");
						strcat(interogare,")");
						if(sqlite3_prepare_v2(db,interogare,-1,&stmt,0)!=SQLITE_OK)
						{
							printf("Eroare la adaugare comentariu:%s",err);
						}
						sqlite3_step(stmt);
						sqlite3_finalize(stmt);
						if(sqlite3_prepare_v2(db,"select comentariu from ComentariiMelodii",-1,&stmt,0)==SQLITE_OK)
						{
							printf("Eroare la select:\n");
						}
						while(sqlite3_step(stmt)!=SQLITE_DONE)
						{
							const unsigned char* d=sqlite3_column_text(stmt,0);
							char b[100]; strcpy(b,d);
						}
						sqlite3_finalize(stmt);
						strcpy(msg,"Comentariul a fost adaugat.\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
					else if(strcmp(comanda,"afisareComentariiMelodie")==0)
					{//ComentariiMelodii(numeUtilizator varchar(20), numeMelodie varchar(40), comentariu varchar(100)
						char msg[1024]; char melodieAleasa[40];
						strcpy(msg,"Introduceti melodia:");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(melodieAleasa,sizeof(melodieAleasa));
						recv(newSocket,melodieAleasa,40,0);
						int melodieCorecta=0;
						while(!melodieCorecta)
						{
							sqlite3_prepare_v2(db,"select numeMelodie from TopMelodii",-1,&stmt,0);
							while(sqlite3_step(stmt)!=SQLITE_DONE)
							{
								const unsigned char* nume; nume=sqlite3_column_text(stmt,0);
								char numeM[40]; strcpy(numeM,nume);
								if(strcmp(numeM,melodieAleasa)==0)
								{
									melodieCorecta=1;
								}
							}
							sqlite3_finalize(stmt);
							if(!melodieCorecta)
							{
								bzero(msg,sizeof(msg));
								strcpy(msg,"Melodie inexistenta! Incercati din nou.\n\n");
								send(newSocket,msg,strlen(msg),0);
								bzero(msg,sizeof(msg));
								bzero(melodieAleasa,sizeof(melodieAleasa));
								recv(newSocket,melodieAleasa,40,0);
							}
						}//ComentariiMelodii(numeUtilizator varchar(20), numeMelodie varchar(40), comentariu varchar(100)
						//strcpy(msg,"Melodie corecta.\n\n");
						//send(newSocket,msg,strlen(msg),0);
						//bzero(msg,sizeof(msg));
						char interogare[100];
						strcpy(interogare,"select numeUtilizator, comentariu from ComentariiMelodii where numeMelodie=");
						strcat(interogare,"'");
						strcat(interogare,melodieAleasa);
						strcat(interogare,"'");
						if(sqlite3_prepare_v2(db,interogare,-1,&stmt,0)!=SQLITE_OK)
						{
							printf("Eroare[BD]:%s",err);
						}
						strcpy(msg,"Comentarii:\n");
						while(sqlite3_step(stmt)!=SQLITE_DONE)
						{
							const unsigned char* numeU; numeU=sqlite3_column_text(stmt,0);
							const unsigned char* comentU; comentU=sqlite3_column_text(stmt,1);
							char nume_U[20]; char coment_U[100]; 
							strcpy(nume_U,numeU); strcpy(coment_U,comentU);
							strcat(msg,"<"); strcat(msg,nume_U); strcat(msg,">");
							strcat(msg,coment_U); strcat(msg,"\n");
						}
						sqlite3_finalize(stmt);
						send(newSocket,msg,strlen(msg),0);
					}
					else if(strcmp(comanda,"stergereMelodie")==0 && utAdmin)
					{
						char msg[100]; char melodieM[40]; bzero(msg,sizeof(msg)); bzero(melodieM,sizeof(melodieM));
						strcpy(msg,"Alegeti melodia pe care doriti sa o stergeti:\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(melodieM,sizeof(melodieM));
						recv(newSocket,melodieM,40,0);
						char interogare[200];
						strcpy(interogare,"DELETE FROM ComentariiMelodii WHERE numeMelodie=");
						strcat(interogare,"'");
						strcat(interogare,melodieM);
						strcat(interogare,"'");
						sqlite3_prepare_v2(db,interogare,-1,&stmt,0);
						sqlite3_step(stmt);
						sqlite3_finalize(stmt);
						strcpy(interogare,"DELETE FROM Melodii WHERE numeMelodie=");
						strcat(interogare,"'");
						strcat(interogare,melodieM);
						strcat(interogare,"'");
						sqlite3_prepare_v2(db,interogare,-1,&stmt,0);
						sqlite3_step(stmt);
						sqlite3_finalize(stmt);
						strcpy(interogare,"DELETE FROM TopMelodii WHERE numeMelodie=");
						strcat(interogare,"'");
						strcat(interogare,melodieM);
						strcat(interogare,"'");
						sqlite3_prepare_v2(db,interogare,-1,&stmt,0);
						sqlite3_step(stmt);
						sqlite3_finalize(stmt);
						strcpy(msg,"Melodia a fost stearsa.\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
					else if(strcmp(comanda,"restrictieVotUtilizator")==0 && utAdmin)
					{
						char msg[40]; char ut[20];
						strcpy(msg,"Introduceti utilizatorul:\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(ut,sizeof(ut));
						recv(newSocket,ut,20,0);
						char interogare[200];
						strcpy(interogare,"INSERT INTO UtilizatoriRestrictionati VALUES");
						strcat(interogare,"(");
						strcat(interogare,"'");
						strcat(interogare,ut);
						strcat(interogare,"'");
						strcat(interogare,")");
						sqlite3_prepare_v2(db,interogare,-1,&stmt,0);
						sqlite3_step(stmt);
						sqlite3_finalize(stmt);
						strcpy(msg,"Utilizatorul a fost restrictionat.\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
					else if(strcmp(comanda,"inlaturareRestrictie")==0)
					{
						char msg[90]; char ut[20];
						strcpy(msg,"Introduceti utilizatorul:\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						bzero(ut,sizeof(ut));
						recv(newSocket,ut,20,0);
						int utilizatorCorect=0;
						while(!utilizatorCorect)
						{
							sqlite3_prepare_v2(db,"select numeUtilizator from UtilizatoriDeSistem",-1,&stmt,0);
							while(sqlite3_step(stmt)!=SQLITE_DONE)
							{
								const unsigned char* nume; nume=sqlite3_column_text(stmt,0);
								char numeM[40]; strcpy(numeM,nume);
								if(strcmp(numeM,ut)==0)
								{
									utilizatorCorect=1;
								}
							}
							sqlite3_finalize(stmt);
							if(!utilizatorCorect)
							{
								bzero(msg,sizeof(msg));
								strcpy(msg,"Utilizator inexistent! Incercati din nou.\n\n");
								send(newSocket,msg,strlen(msg),0);
								bzero(msg,sizeof(msg));
								bzero(ut,sizeof(ut));
								recv(newSocket,ut,20,0);
							}
						}
						if(sqlite3_prepare_v2(db,"DELETE FROM UtilizatoriRestrictionati WHERE numeUtilizator= ?;",-1,&stmt,NULL)==SQLITE_OK)
						{
							sqlite3_bind_text(stmt, 1, ut, -1, NULL);
							sqlite3_step(stmt);
							sqlite3_finalize(stmt);
						}
						else{
							printf("Eroare [BD]\n");
						}
						strcpy(msg,"Restrictia a fost inlaturata.\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
					else if(strcmp(comanda,"afisareInformatiiMelodie")==0)
					{
						char msg[1024]; char melodieAleasa[40]; int melodieCorecta=0;
						char interogare[50];
						bzero(msg,sizeof(msg));
						bzero(melodieAleasa,sizeof(melodieAleasa));
						strcpy(msg,"Alegeti melodia:\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						recv(newSocket,melodieAleasa,40,0);
						while(!melodieCorecta)
						{
							sqlite3_prepare_v2(db,"select numeMelodie from TopMelodii",-1,&stmt,0);
							while(sqlite3_step(stmt)!=SQLITE_DONE)
							{
								const unsigned char* nume; nume=sqlite3_column_text(stmt,0);
								char numeM[40]; strcpy(numeM,nume);
								if(strcmp(numeM,melodieAleasa)==0)
								{
									melodieCorecta=1;
								}
							}
							sqlite3_finalize(stmt);
							if(!melodieCorecta)
							{
								bzero(msg,sizeof(msg));
								strcpy(msg,"Melodie inexistenta! Incercati din nou.\n\n");
								send(newSocket,msg,strlen(msg),0);
								bzero(msg,sizeof(msg));
								bzero(melodieAleasa,sizeof(melodieAleasa));
								recv(newSocket,melodieAleasa,40,0);
							}
						}
						//Melodii(numeMelodie varchar(40), descriere varchar(100), link varchar(120))",NULL,NULL,&err);
						//ComentariiMelodii(numeUtilizator varchar(20), numeMelodie varchar(40), comentariu varchar(100))",NULL,NULL,&err);
						if(sqlite3_prepare_v2(db,"select numeMelodie, descriere, link from Melodii where numeMelodie= ?",-1,&stmt,NULL)==SQLITE_OK)
						{
							sqlite3_bind_text(stmt, 1, melodieAleasa, -1, NULL);
						}
						else{
							printf("Eroare [BD]\n");
						}
						sqlite3_step(stmt);
						const unsigned char *mel; mel=sqlite3_column_text(stmt,0); char v1[50]; strcpy(v1,mel);
						const unsigned char *desc; desc=sqlite3_column_text(stmt,1); char v2[100]; strcpy(v2,desc);
						const unsigned char *link; link=sqlite3_column_text(stmt,2); char v3[120]; strcpy(v3, link);
						sqlite3_step(stmt);
						sqlite3_finalize(stmt);
						bzero(msg,sizeof(msg));
						strcpy(msg,"Descriere melodie:\n");
						strcat(msg,v2);
						strcat(msg,"\n");
						strcat(msg,"Link melodie:\n");
						strcat(msg,v3);
						strcat(msg,"\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
					else if(strcmp(comanda,"iesire")==0)
					{
						char msg[15];
						strcpy(msg,"OK, iesire");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
						iesire=1;
						break;
					}
					else{
						char msg[40];
						strcpy(msg,"Comanda incorecta! Incercati din nou.\n\n");
						send(newSocket,msg,strlen(msg),0);
						bzero(msg,sizeof(msg));
					}
				}
				if(iesire==1){
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}
			}
		}
	}
	close(newSocket);
	return 0;
}
