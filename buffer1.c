//git version 1.7.9.5
//Buffer1.c : v1.0 (version 1.0)
//gcc (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3


#include"buffer1.h"

void strVazia(int tam){
	//define todas strings como vazia
    int i;

    for(i = 0;i<tam;i++){
            vr[i].nome[0] = '\0';
    }
}

void inicializaBuffer(buffer *p){
    int i;

    for(i = 0;i< NUMP;i++){
            p->dirtybit = 0;
            p->numbytes = 0;
            p->numRegs = 0;
            p->pincount = 0;
            p->data[0] = '\0';//define a string vazia
    }
}
void orgOnBuffer(int next,buffer *b,char *tuple){
	
	int i = 0;
	int j = next;
		
	for(;j < next + b->numbytes;i++,j++){
		//copia a tupla para o buffer
		b->data[j] = tuple[i];
	}
	
}

void cpyvar(char *tuple,char *vd,int init,int tam){
		//copiar valores tipo double e int para a string
		
		int i=init,j=0;
		for (;i<tam+init;i++)
			tuple[i]=vd[j++];
}

void concatString(char *string1, char *string2, int pos){
	//puts(string2);
	
	int i;
	int j;
	int v = strlen(string2);
	
	for(i = pos,j = 0;j <= v ; i++,j++){
		string1[i] = string2[j];
		
	}
	string1[i] = '\0';
	
}

char *aloca(char *fn){
	if(fn)
		free(fn);
	
	fn = (char *)malloc(sizeof(char)*(tamanho));
	
	return fn;
}

int cpytuple(buffer *b,char *tuple,FILE *p){

		int o;
		int count = 0;
		int jj = 0;
		int tTam = 0; //variais de controle
		
		
		char *bh = NULL;
		bh = aloca(bh);
		
		union c_double vdouble;
		union c_int vint;
		
		
		
		for(o = 0;o<tamanho;o++){
			if(vr[jj].tipo == 'S'){
				if(tTam == 0){
					tTam = vr[jj].tam;
					
				}
				if(tTam != vr[jj].tam){
					//copia uma string de tamanho definido
					tTam = vr[jj].tam;
					fread(bh,sizeof(char),vr[jj].tam,p);
					o = (vr[jj].tam + o) - 1;
					count = count + vr[jj].tam;
					concatString(tuple,bh,o - vr[jj].tam +1);
					bh = aloca(bh);
				}
				else{
					//copia string
					fread(bh,sizeof(char),vr[jj].tam,p);
					o = (vr[jj].tam + o) -1;
					count = count + vr[jj].tam;
					concatString(tuple,bh, o - vr[jj].tam +1);
					bh = aloca(bh);
				}
			}
			else if(vr[jj].tipo == 'D'){
				double k;
				//copia do arquivo um dado double
				if(tTam != vr[jj].tam)
					tTam = vr[jj].tam;
				fread(&k,sizeof(double),1,p);
				 vdouble.numd = k;
				 cpyvar(tuple,vdouble.cnumd,o,sizeof(double));
				 o = (vr[jj].tam + o) - 1;
				count = count + vr[jj].tam;
				
				
			}
			else{
				int l;
				//copia um dado tipo int
				if(tTam != vr[jj].tam)
					tTam = vr[jj].tam;
				fread(&l,sizeof(int),1,p);
				vint.num = l;
				
				cpyvar(tuple,vint.cnum,o,sizeof(int));
				o = (vr[jj].tam + o) - 1;
				count = count + vr[jj].tam;
			}
				
			if(count >=vr[jj].tam-1){
				jj++;
				count = 0;
			}
		}
		
		free(bh);
		if(cpytoBuffer(b,tuple) == 0)
			return 0;
		
		return -1;
}

int printBuffer(buffer *b, int page, int tuple){
	
	int i,y;
	int j = 0;
	int count = 0;
	int inicio = 0;
	
	if (page<0 || page>=SIZE){
		//caso o numero da pagina for invalido
		return -1;
	}
	if(b[page].numbytes == 0){
		//retorna se a pagina estiver vazia
		return -1;
	}
	if(b[page].numRegs  < tuple || tuple < 1){
		//retorna se a tupla nao existir
		return -1;
	}
	int acha;
	int tuplesTam;
	
	for(acha = 1;acha <= tuple; acha ++ ){
		//procura a posicao da tupla no buffer
		if(acha == tuple)
			tuplesTam = tamanho * acha;
	}
	tuplesTam = tuplesTam - tamanho;
	
	for(i = tuplesTam,y = 0; j < att; i++,y++){
		if( count == 0){
			printf("\n %s : ",vr[j].nome);
			inicio = i;
		}
		if(vr[j].tipo == 'S' ){
			if(b[page].data[i]=='\0'){
				y = vr[j].tam - 1;
				i = i + (vr[j].tam - (i - inicio)) -1;
				/*Quando um '\0' for encontrado, indica que o valor dentro do atributo terminou,
				 e o restante dos bytes que sobraram serao pulados ate o proximo atributo*/
			}
			else{
				printf("%c", b[page].data[i]);
			}
		count++;
		}
		else if(vr[j].tipo == 'D'){
			//imprime um numero double
			double *d = (double *)&b->data[i];
			printf("%f ",*d);
			i = i + sizeof(double) -1;
			count++;
			y = sizeof(double)-1;
		}
		else{
				//imprime um numero int
				int *x = (int *)&b->data[i];
				printf("%d", *x);count++;
				i = i + sizeof(int) -1;
				y = sizeof(int)-1;
		}	
		if(vr[j].tam-1 == y){
			//testa se o atributo terminou 
			j++;
			y  = 0;
			count = 0;
		}
	}
	return 0;
}

int bufferpool(buffer *b, char *arqMeta,char *arqDados){
    FILE *p = fopen(arqMeta,"rb");

		if(!p){
			//retorna se encontrar erro ao abrir o arquivo
			return -1;
		}
		
		int l = 0;
		int i = 0;

		char aux='a'; //inicializa com valor qualquer
		char aux1[2];

        fread(&att,sizeof(int),1,p); //Recebe o numero de atributos

		strVazia(att);//Define que todas as strings do tipo nome estão vazias


		while(i < att){
			while(aux != '\0'){
				//Procura uma string no arquivo
				fread(&aux,sizeof(char),1,p);
				aux1[0] = aux;
				aux1[1] = '\0'; //transforma aux em uma string para uso da funcao strcat
				strcat(vr[l].nome,aux1);//concatena a string nome com aux1 enquanto o while for verdadeiro
			}
			

			fread(&vr[l].tipo,sizeof(char),1,p);
			//Le o tipo do atributo
			

			fread(&vr[l].tam,sizeof(int),1,p);
			//le o tamnho do atributo
              
			l++;
			i++;
			aux = 'a';//seta aux para que entre no while interno na proxima execucao
			
		}

		fclose(p);

        
		p = fopen(arqDados,"r+");


		if(!p){
			//retorna se encontrar erro ao abrir o arquivo
			return -1;

        }
		int g = 0;
		int getMax = 0;
		int o; // controle
		
		
		fseek(p,0,SEEK_END);
		long pos = ftell(p); //pega o tamanho do arquivo
		rewind(p);
		
		for(g=0;g<att;g++){
			getMax = getMax + vr[g].tam;//pega tamanho da tupla
		}
		tamanho = getMax;
		getMax = pos/getMax;//numero de tuplas
        
        char *tuple = NULL;
        tuple = aloca(tuple);
        
        for(o = 0;o<getMax;o++){
			cpytuple(b,tuple,p);
		}
		 
		 free(tuple);
		 fclose(p); 
		 return 0;
}
int cpytoBuffer(buffer *b,char *tuple){
	
	int i=0;
	int disp; //controle
	int flag = 0;
    

	while(!flag && (i < NUMP)){
		if(b[i].numRegs == 0){ //se nao houver tuplas na pagina, ela estara disponivel
			disp = SIZE;
			flag = 1;
			break;
		}
		else 
			disp = SIZE - b[i].numRegs*(tamanho);
			 //senao, sera calculado o espaco livre na pagina
				
		if(flag == 0 && ((tamanho) < disp)){ //se a pagina nao esta vazia, entao sera verificado o espaco disponivel
			flag = 1;
			break;
		}
		i++;
	}
	if(flag){
		
		int next = b[i].numRegs*(tamanho); //proxima posicao disponivel 
		
		b[i].numRegs++; //incrementa o contador de registros na pagina
		b[i].numbytes = (tamanho); 
		
		orgOnBuffer(next,&b[i],tuple);
		return 0;
	}
	return -1;
}
int seekFiles(char *tableName)
{
	char temp[256];
	int numberOfTables;
	FILE *dictionary = fopen("fs_dictionary.dat","r+");
	if(dictionary == NULL)
		return -2;//Retorna 1 se o arquivo de dicionario de dados não foi encontrado.
	fread(&numberOfTables,sizeof(int),1,dictionary);
	char stringData[50];
	int tableID = 37;
	int i,j,breakPoint = 0;
	for(i = 0; i < numberOfTables; i++)
	{
		for(j = 0;breakPoint == 0; j++)
		{
			fread(&stringData[j],1,1,dictionary);
			if(stringData[j] == '\0')
				breakPoint = j;
		}
		breakPoint = 0;
		fread(&tableID,sizeof(int),1,dictionary);
		if(strcmp(stringData,tableName) == 0)
			return tableID;//Caso a tabela seja encontrada ele retorna o ID da mesma.
	}
	return -1;//Quer dizer que a tabela não foi encontrada no dicionário de dados
}
