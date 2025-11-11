void main(){
    int i, j, x=0, y=0;
    for(i=0;i<1000;i++){
        for(j=0;j<10;j++){
            if((i+j)&1) x++;
            else y++;
        }
    }
}