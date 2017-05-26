
#include "crc.hpp"
#include <array>
#include <iostream>


/*
 * this function gets two polynoms represented as array of integer and making xor
 * on their x.
 */
void subtract (int* a, int len1, int* b, int len2, int* s){
	for(int i=0; i<len2; i++){
		s[i]=(a[i]+b[i])%2; //a-b
	}
	for(int i=len2; i<len1; i++){
		s[i]=a[i];
	}
}

/*
 * this function gets a polynom represented as array of integer and multiply it
 * with x in deg.
 */
void multiply (int* p, int degOfP, int deg ,int* m){
	for(int i=0; i<=degOfP; i++){
		m[i+deg]=p[i];
	}
	for(int i=0; i<deg; i++){
		m[i]=0;
	}
}

void reminder (int* t, int len1, int* p, int len2, int g, int* res){
	int maxDeg=-1;
	for(int i=0; i<len1; i++){
		maxDeg=(t[i]==1 ? i : maxDeg);
	}
	while(maxDeg>=g){
		int multiplyer=maxDeg-g;
		int subtractor[len1];
		for(int i=0; i<len1;i++){
			subtractor[i]=0;
		}
		multiply (p, g, multiplyer ,subtractor);
		int newT[len1];
		for(int i=0; i<len1;i++){
			newT[i]=0;
		}
		subtract (t, len1, subtractor, len1, newT);
		for(int i=0; i<len1; i++){
			t[i]=newT[i];
		}
		int newMaxDeg=0;
		for(int i=0; i<len1; i++){
			newMaxDeg=(t[i]==1 ? i : newMaxDeg);
		}
		maxDeg=newMaxDeg;
	}
	for(int i=0; i<len1; i++){
		res[i]=t[i];
	}
}

/*
 * this function gets a number in hexa and return a array of the number in binary.
 * p[0]=LSB
 * p[len-1]=MSB
 */
void pToBoolArray(int num, int p[], int g){
	int i=0;
	while(num>0){
		if(num%2==1){
			p[i++]=1;
		}
		else{
			p[i++]=0;
		}
		num/=2;
	}
	p[g]=1;
}

int charToInt(char ch){
	char fourLow= ch & 0x0F;
	char fourHigh= (ch>>4)&0x0F;
	return (int)fourHigh*16+(int)fourLow;
}

void stringToBitArray(unsigned char* ch, int len, int* arr){
	int count=0;
	for(int i=len-1; i>=0; i--){
		int num=charToInt(ch[i]);
		for(int j=0; j<8; j++){
			if(num%2==1){
				*(arr+count)=1;
			}
			else{
				*(arr+count)=0;
			}
			num/=2;
			count++;
		}
	}
}

char concatateChars(int num1, int num2){
	//asuuming that each num is less then 16!!!
	char c1=(char)(num1);
	char c2=(char)(num2);
	return (c1<<4)|c2;
}

void bitArrayToString(unsigned char* res, int len, int* arr, int histoLen){
	int count=0;
	for(int i=len-1; i>=0; i--){
		int num2=0;
		if(count<histoLen){
			num2+=arr[count];
		}
		if(count+1<histoLen){
			num2+=2*arr[count+1];
		}
		if(count+2<histoLen){
			num2+=4*arr[count+2];
		}
		if(count+3<histoLen){
			num2+=8*arr[count+3];
		}
		int num1=0;
		if(count+4<histoLen){
			num1+=arr[count+4];
		}
		if(count+5<histoLen){
			num1+=2*arr[count+5];
		}
		if(count+6<histoLen){
			num1+=4*arr[count+6];
		}
		if(count+7<histoLen){
			num1+=8*arr[count+7];
		}
		res[i]=concatateChars(num1, num2);
		count+=8;
	}
}


/*
 * param msg - Pointer to message to encode
 * param len - Length of message in bytes
 * p - Polynom value
 * g - Polynom grade
 * Output param e_msg - Pointer to message with CRC
 * Return: void
 * */
void encode(unsigned char* msg, int len, unsigned int p, int g ,unsigned char* e_msg){

	if(p==0){
		int histo1[8*len];
		stringToBitArray(msg, len ,histo1);
		int histo2[8*len+g+1];
		for(int i=0; i<=g;i++){
			histo2[i]=0;
		}
		for(int i=g+1; i<=8*len;i++){
			histo2[i]=histo1[i-g-1];
		}
		bitArrayToString(e_msg, len+(g/8)+1, histo2, 8*len+g+1);
		return;
	}

	int arr[8*len];
	int p_x[g+1];
	stringToBitArray(msg, len ,arr);
	pToBoolArray(p, p_x,  g);
	int res[g+1];
	int newMessegeInBit[8*len+g];
	for(int i=0;i <8*len; i++){
		newMessegeInBit[i+g]=arr[i];
	}
	for(int i=0; i<g; i++){
		newMessegeInBit[i]=0;
	}
	reminder(newMessegeInBit, 8*len+g, p_x, g+1, g, res);
	for(int i=0;i <8*len; i++){
		newMessegeInBit[i+g]=arr[i];
	}
	for(int i=0; i<g; i++){
		newMessegeInBit[i]=res[i];
	}
	bitArrayToString(e_msg, len+(g/8)+1, newMessegeInBit, 8*len+g);
}

/*
 * param msg - Pointer to message with CRC
 * param len - Length of message
 * p - Polynom value
 * g - Polynom grade
 * Return:  1 - CRC is OK, 0 - CRC Error
 * */
bool validate(unsigned char* msg, int len, unsigned int p, int g){
	if(p==0){
		int histo[8*len];
		stringToBitArray(msg, len ,histo);
		for(int i=0; i<=g; i++){
			if(histo[i]!=0){
				return 0;
			}
		}
		return 1;
	}
	int arr[8*len];
	int p_x[g+1];
	stringToBitArray(msg, len ,arr);
	pToBoolArray(p,  p_x,  g);
	int* res= new int[8*len];
	reminder(arr, 8*len, p_x, g+1, g, res);
	for(int i=0; i<g; i++){
		if(res[i]!=0){
			return 0;
		}
	}
	return 1;
}
/*
int main(){
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	unsigned char msg[1]={0x25};
	unsigned char e_msg[2];

	encode(msg, 1, 0x15, 4 , e_msg);

	for(int i=0;i<2;i++){
		printf("%d\n", e_msg[i]);

	}

	bool flag=validate( e_msg, 2, 0x15, 4);

	printf("%d\n", flag);
	return 0;
}*/



