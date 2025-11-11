#define MOD 12289
#define root 11

int expo(int a, int b, int mod){
    int ans=1; 
    while(b){
        if(b&1) ans=(ans*1ll*a)%mod; 
        a=(a*1ll*a)%mod; b>>=1;
    } 
    return ans;
}

int mod_div(int a, int b, int mod){
    return (a*1ll*expo(b,mod-2,mod))%mod;
}

#define n 1729
#define m 1000
#define N 4096
int rt[20];

void precompute(){
    int i,k,s;
    rt[0] = rt[1] = 1;
    for(k=2,s=2;k<n;k*=2,s++){
        int z1 = expo(root, MOD>>s, MOD);
		for(i=k;i<2*k;i++){
            if(i&1) rt[i]=rt[i/2]*z1%MOD;
            else rt[i] = rt[i/2];
        }
	}
}

void ntt(int a[]){  
	int L = 0;
    int NN = N;
    int i,j,k,rev[N];
    while(NN>>=1) ++L;
	
	for(i=0;i<N;i++){
		rev[i]=(rev[i/2]|(i&1)<<L)/2;
		if(i<rev[i]){
            int temp = a[i];
            a[i] = a[rev[i]];
            a[rev[i]] = temp;
        }
	}

	for(k=1;k<N;k*=2){
        for(i=0;i<N;i+=2*k){
            for(j=0;j<k;j++){
                int z=rt[j+k]*a[i+j+k]%MOD;
                int tmp=a[i+j];
                a[i+j+k]=tmp-z+(z>tmp? MOD:0);
                a[i+j]=tmp+z-(tmp+z>=MOD? MOD:0);
            }
        }
    }
}

void brutemul(int a[], int b[], int ans[]){
    int i,j;
    for(i=0;i<n+m;i++) ans[i] = 0;
	for(i=0;i<n;i++){
        for(j=0;j<m;j++){
            ans[i+j]=(ans[i+j]+(a[i]*b[j]%MOD))%MOD;
        }
    }
}

void mul(int a[], int b[], int ans[]){
	int inv=mod_div(1,N,MOD);
    int L[N], R[N], out[N];
    int i;
	if((n==0) || (m==0)) return;
	if((n<32) || (m<32)){
        brutemul(a,b,ans);
        return;
    }
    for(i=0;i<n;i++) L[i] = a[i];
    for(;i<N;i++) L[i] = 0;
    for(i=0;i<m;i++) R[i] = b[i];
    for(;i<N;i++) R[i] = 0;
    
	ntt(L); ntt(R);
	for(i=0;i<N;i++) out[-i&(N-1)]=L[i]*R[i]%MOD*inv%MOD;
	ntt(out);
    for(i=0;i<n+m;i++) ans[i] = out[i];
}

void main(){
    int a[n], b[m];
    int ans[n+m];
    int i,val=1;
    precompute();
    for(i=0;i<n;i++,val++) a[i] = val;
    for(i=0;i<m;i++,val++) b[i] = val;
    mul(a,b,ans);
}