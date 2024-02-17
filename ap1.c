#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct token
{
    unsigned int addr;
    char label[1000];
    char opcode[1000];
    char operand[1000];
    char error[1000];
};
int searchFile(const char *filename,const char *word)
{
    FILE *file=fopen(filename,"r");
    if(file==NULL)
    {
        printf("Error opening file");
        exit(1);
    }
    char line[1000];
    int found=0;
    while(fgets(line,sizeof(line),file)!=NULL)
    {
        if(strstr(line,word)!=NULL)
        {
            found=1;
            break;
        }
    }
    fclose(file);
    return found;
}
void tokenise(char line[1000],struct token *token)
{
    int result;
    result=sscanf(line,"%s %s %s",token->label,token->opcode,token->operand);
    if(result==2)
    {
        char temp[1000],temp1[1000];
        strcpy(temp1,token->opcode);
        strcpy(temp, token->label);
        strcpy(token->opcode,temp);
        strcpy(token->operand,temp1);
        strcpy(token->label,"");
        strcpy(token->error,"");
    }
    else if(result==1)
    {
        strcpy(token->opcode,token->label);
        strcpy(token->operand,"");
        strcpy(token->label,"");
        strcpy(token->error,"");
    }
}

int main()
{
    FILE *f1=fopen("input.txt","r");
    FILE *f3=fopen("imt.txt","w");
    struct token token;
    unsigned int c;
    unsigned int start_addr;
    unsigned int program_length;
    if(f1==NULL)
    {
        printf("Error in opening input file\n");
        exit(1);
    }
    if(f3==NULL)
    {
        printf("Error in opening intermediate file\n");
        exit(1);
    }
    char line[1000];
    fgets(line,1000,f1);
    tokenise(line,&token);
    if (strcmp(token.opcode,"START")==0)
    {
        c=start_addr=strtol(token.operand,NULL,16);
        //c=start_addr;
        //printf("%x\n",c);
        token.addr=c;
        fprintf(f3,"%x %s %s %s %s\n",token.addr,token.label,token.opcode,token.operand,token.error);
        fgets(line,1000,f1);
        tokenise(line,&token);
        //fprintf(f3,"%x %s %s %s %s\n",token.addr,token.label,token.opcode,token.operand,token.error);
    }
    else
    {
        c=0x00;
    }
    while(strcmp(token.opcode,"END")!=0)
    {
        token.addr=c;
        if(token.label[0]!='\0')
        {
            int s=searchFile("symtab.txt",token.label);
            if(s==1)
            {
                strcpy(token.error,"Duplicate symbol");
            }
            else
            {
                FILE *f4=fopen("symtab.txt","a");
                //token.addr=c;
                fprintf(f4,"%s %x\n",token.label, c);
                fclose(f4);
            }
        }

        int o=searchFile("optab.txt",token.opcode);
        if(o==1)
            c=c+3;
        else if(strcmp(token.opcode,"WORD")==0)
            c=c+3;
        else if(strcmp(token.opcode,"RESW")==0)
        {
            c+=(3*(atoi(token.operand)));
        }
        else if(strcmp(token.opcode,"RESB")==0)
        {
            unsigned int b=atoi(token.operand);
            c=c+b;
        }
        else if(strcmp(token.opcode,"BYTE")==0)
        {
            if(token.operand[0]=='C')
                    c+=strlen(token.operand)-7;
            else if(token.operand[0]=='X')
                    c+=floor((strlen(token.operand)-7)/2);

        }
        else if(strcmp(token.opcode,"ORG")==0)
        {
            c=strtol(token.operand,NULL,16);
        }
        else
            strcpy(token.error,"Invalid operation code");
        //token.addr=c;
        fprintf(f3,"%x %s %s %s %s\n",token.addr,token.label,token.opcode,token.operand,token.error);
        fgets(line,1000,f1);
        tokenise(line,&token);
    }
    c=token.addr;
    fprintf(f3,"   %s %s %s %s\n",token.label,token.opcode,token.operand,token.error);
    printf("Starting address =%x\nEnd address=%x\n",start_addr,c);
    program_length=c-start_addr;
    printf("\n-------------------------------LENGTH OF PROGRAM = %x---------------------------------\n\n",program_length);
    fclose(f1);
    fclose(f3);
    return 0;
}
