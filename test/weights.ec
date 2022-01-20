#
# weights.ec
#
# Calculates what 3 weights are needed to weigh all possible weights
# from 1 to 14 kg (whole kilos only) with only a balance scale
#

weights={a=1;b=1;c=1;s=0;\
         while(c<=14,\
             s=1;m=0;\
             while(s<14,if(!m=weighttest(a,b,c,s),break);s=s+1);\
             if(m,"Found:\n";print(a,b,c));\
             a=a+1;if(a==14,a=1;b=b+1);if(b==14,b=1;c=c+1)\
         );\
         "Done!\n"\
}

weighttest={\
    s1=-1;s2=-1;s3=-1;\
    while(s3<2,\
        if((s1*$1+s2*$2+s3*$3)==$4,\
            return 1,\
            s1=s1+1;if(s1==2,s1=-1;s2=s2+1);if(s2==2,s2=-1;s3=s3+1)\
        )\
    );\
    return 0\
}

"\n"
"This program calculates what 3 weights are needed to weigh\n"
"all possible weights from 1 to 14 kg (in whole kilos)\n\n"
"Searching all combinations of three weights from 1-14 kg...\n\n"
weights()
