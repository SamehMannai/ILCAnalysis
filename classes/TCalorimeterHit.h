#ifndef _TCalorimeterHit_hh_
#define _TCalorimeterHit_hh_
#include <EVENT/RawCalorimeterHit.h>

class TCalorimeterHit
{
public:
  TCalorimeterHit() {};

  static int event_count;
  int nhit_count;
  TCalorimeterHit(const EVENT::RawCalorimeterHit &a_hit) {
    nhit_count=0; 
		//		event_count==NEvent;
		//	if(event_count>0)
			{

		Int_t DifZ, DifX;
		
 
	
	  Int_t ID0;
    ID0=a_hit.getCellID0();
    fDif_id=-5;
    fDif_id=ID0 & 0xFF; // Dif id
    fAsic_id=(ID0 & 0xFF00)>>8; // Asic id
    fChan_id=(ID0 & 0x3F0000)>>16; // Channel id
	
	
fX = (1+MapILargeHR2[fChan_id]+AsicShiftI[fAsic_id])*1.04125; // X coordinate (cm)
    
fY = (32-(MapJLargeHR2[fChan_id]+AsicShiftJ[fAsic_id]))*1.04125; //Y coordinate (cm)

 fZ=-1.;
    fThr0=0;fThr1=0;fThr2=0;// Thresholds
		if(fDif_id==181) DifX=64, DifZ=0; 
		if(fDif_id==94) DifX=32, DifZ=0; 
		if(fDif_id==30) DifX=0, DifZ=0; 	
	 
  	if(fDif_id==117) DifX=64, DifZ=35; 
		if(fDif_id==149) DifX=32, DifZ=35; 
		if(fDif_id==115) DifX=0, DifZ=35; 

		if(fDif_id==122) DifX=64, DifZ=2; 
		if(fDif_id==123) DifX=32, DifZ=2; 
		if(fDif_id==130) DifX=0, DifZ=2; 

  

  	if(fDif_id==129) DifX=64, DifZ=3; 
		if(fDif_id==118) DifX=32, DifZ=3; 
		if(fDif_id==119) DifX=0, DifZ=3; 


   	if(fDif_id==164) DifX=64, DifZ=4; 
		if(fDif_id==152) DifX=32, DifZ=4; 
		if(fDif_id==151) DifX=0, DifZ=4; 	


  	if(fDif_id==74) DifX=64, DifZ=5; 
		if(fDif_id==61) DifX=32, DifZ=5; 
		if(fDif_id==75) DifX=0, DifZ=5; 	


	  if(fDif_id==156) DifX=64, DifZ=6; 
 		if(fDif_id==111) DifX=32, DifZ=6; 
		if(fDif_id==110) DifX=0, DifZ=6; 	


  	if(fDif_id==102) DifX=64, DifZ=7; 
		if(fDif_id==177) DifX=32, DifZ=7; 
		if(fDif_id==103) DifX=0, DifZ=7; 	


  	if(fDif_id==133) DifX=64, DifZ=8; 
		if(fDif_id==136) DifX=32, DifZ=8; 
		if(fDif_id==134) DifX=0, DifZ=8;



  	if(fDif_id==128) DifX=64, DifZ=9; 
		if(fDif_id==120) DifX=32, DifZ=9; 
		if(fDif_id==121) DifX=0, DifZ=9; 	 	

  	if(fDif_id==65) DifX=64, DifZ=10; 
		if(fDif_id==64) DifX=32, DifZ=10; 
		if(fDif_id==58) DifX=0, DifZ=10; 	

  	if(fDif_id==148) DifX=64, DifZ=11; 
		if(fDif_id==72) DifX=32, DifZ=11; 
		if(fDif_id==73) DifX=0, DifZ=11; 	


  	if(fDif_id==78) DifX=64, DifZ=12; 
		if(fDif_id==79) DifX=32, DifZ=12; 
		if(fDif_id==60) DifX=0, DifZ=12; 	


	  if(fDif_id==44) DifX=64, DifZ=13; 
		if(fDif_id==43) DifX=32, DifZ=13; 
		if(fDif_id==113) DifX=0, DifZ=13;

  	if(fDif_id==66) DifX=64, DifZ=14; 
		if(fDif_id==69) DifX=32, DifZ=14; 
		if(fDif_id==68) DifX=0, DifZ=14;


  	if(fDif_id==154) DifX=64, DifZ=15; 
		if(fDif_id==127) DifX=32, DifZ=15; 
		if(fDif_id==186) DifX=0, DifZ=15; 	


  	if(fDif_id==147) DifX=64, DifZ=16; 
		if(fDif_id==70) DifX=32, DifZ=16; 
		if(fDif_id==71) DifX=0, DifZ=16; 	


  	if(fDif_id==47) DifX=64, DifZ=17; 
		if(fDif_id==139) DifX=32, DifZ=17; 
		if(fDif_id==140) DifX=0, DifZ=17; 	




  	if(fDif_id==143) DifX=64, DifZ=18; 
		if(fDif_id==77) DifX=32, DifZ=18; 
		if(fDif_id==76) DifX=0, DifZ=18; 


  	



  	if(fDif_id==159) DifX=64, DifZ=19; //problème
		if(fDif_id==91) DifX=32, DifZ=19; 
		if(fDif_id==36) DifX=0, DifZ=19; 		

     if(fDif_id==179) DifX=64, DifZ=20; 
		if(fDif_id==178) DifX=32, DifZ=20; 
		if(fDif_id==183) DifX=0,  DifZ=20; 	

  	
  	if(fDif_id==41) DifX=64, DifZ=21; 
		if(fDif_id==42) DifX=32, DifZ=21; 
		if(fDif_id==67) DifX=0, DifZ=21; 	



  	if(fDif_id==137) DifX=64, DifZ=22; 
		if(fDif_id==46) DifX=32, DifZ=22; 
		if(fDif_id==138) DifX=0, DifZ=22; 	

		if(fDif_id==131) DifX=64, DifZ=23; 
		if(fDif_id==173) DifX=32, DifZ=23; 
		if(fDif_id==144) DifX=0, DifZ=23; 	



   	


    if(fDif_id==189) DifX=64, DifZ=24; 
		if(fDif_id==184) DifX=32, DifZ=24; 
		if(fDif_id==160) DifX=0, DifZ=24; 	
   

  	if(fDif_id==172) DifX=64, DifZ=25; 
		if(fDif_id==167) DifX=32, DifZ=25; 
		if(fDif_id==171) DifX=0, DifZ=25; 	

  	if(fDif_id==146) DifX=64, DifZ=26; 
		if(fDif_id==135) DifX=32, DifZ=26; 
		if(fDif_id==145) DifX=0, DifZ=26; 	

		

  	if(fDif_id==185) DifX=64, DifZ=27; 
		if(fDif_id==170) DifX=32, DifZ=27; 
		if(fDif_id==180) DifX=0, DifZ=27; 	

		
  	if(fDif_id==187) DifX=64, DifZ=28; //problème
		if(fDif_id==188) DifX=32, DifZ=28; // 
		if(fDif_id==190) DifX=0, DifZ=28; //	


		

	  if(fDif_id==169) DifX=64, DifZ=29; //problème
		if(fDif_id==165) DifX=32, DifZ=29; //
		if(fDif_id==166) DifX=0, DifZ=29; 	//

  	if(fDif_id==155) DifX=64, DifZ=30; 
		if(fDif_id==57) DifX=32, DifZ=30; 
		if(fDif_id==50) DifX=0, DifZ=30; 	


	  if(fDif_id==25) DifX=64, DifZ=31; 
		if(fDif_id==108) DifX=32, DifZ=31; 
		if(fDif_id==153) DifX=0, DifZ=31; 	

	  if(fDif_id==51) DifX=64, DifZ=32; 
		if(fDif_id==56) DifX=32, DifZ=32; 
		if(fDif_id==109) DifX=0, DifZ=32; 	

	 

	  if(fDif_id==116) DifX=64, DifZ=33; 
		if(fDif_id==150) DifX=32, DifZ=33; 
		if(fDif_id==107) DifX=0, DifZ=33; 	//problème

  	if(fDif_id==126) DifX=64, DifZ=34; 
		if(fDif_id==124) DifX=32, DifZ=34; 
		if(fDif_id==49) DifX=0,  DifZ=34; 	

	  if(fDif_id==174) DifX=64, DifZ=1; 
		if(fDif_id==175) DifX=32, DifZ=1; 
		if(fDif_id==176) DifX=0,  DifZ=1; //problème	


		
  	if(fDif_id==48) DifX=64, DifZ=36; 
		if(fDif_id==45) DifX=32, DifZ=36; 
		if(fDif_id==114) DifX=0, DifZ=36; 	


	  if(fDif_id==98) DifX=64, DifZ=37; 
		if(fDif_id==93) DifX=32, DifZ=37; 
		if(fDif_id==40) DifX=0, DifZ=37; 	

	  if(fDif_id==92) DifX=64, DifZ=38; 
		if(fDif_id==97) DifX=32, DifZ=38; 
		if(fDif_id==100) DifX=0, DifZ=38; 	

		
	 

	  if(fDif_id==62) DifX=64, DifZ=39; 
		if(fDif_id==106) DifX=32, DifZ=39; 
		if(fDif_id==132) DifX=0, DifZ=39; 


		  	if(fDif_id==101) DifX=64, DifZ=40; 
			if(fDif_id==35) DifX=32, DifZ=40; 
		if(fDif_id==99) DifX=0,  DifZ=40; 	

	  if(fDif_id==158) DifX=64, DifZ=41; 
		if(fDif_id==142) DifX=32, DifZ=41; 
		if(fDif_id==141) DifX=0,  DifZ=41; 
	
	  if(fDif_id==163) DifX=64, DifZ=42; 
		if(fDif_id==161) DifX=32, DifZ=42; 
		if(fDif_id==162) DifX=0,  DifZ=42; 

		  if(fDif_id==104) DifX=64, DifZ=43; 
		if(fDif_id==29) DifX=32, DifZ=43; 
		if(fDif_id==112) DifX=0,  DifZ=43; 
	



	  if(fDif_id==59) DifX=64, DifZ=44; 
		if(fDif_id==53) DifX=32, DifZ=44; 
		if(fDif_id==54) DifX=0, DifZ=44; 


		  	if(fDif_id==96) DifX=64, DifZ=45; 
			if(fDif_id==90) DifX=32, DifZ=45; 
		if(fDif_id==27) DifX=0,  DifZ=45; 	

	  if(fDif_id==95) DifX=64, DifZ=46; 
		if(fDif_id==8) DifX=32, DifZ=46; 
		if(fDif_id==5) DifX=0,  DifZ=46; 


     if(fDif_id==18) DifX=64, DifZ=47; 
    if(fDif_id==87) DifX=32, DifZ=47; 
    if(fDif_id==63) DifX=0,  DifZ=47; 


	
	  if(fDif_id==241) DifX=64, DifZ=49; 
		if(fDif_id==242) DifX=32, DifZ=49; 
		if(fDif_id==243) DifX=0,  DifZ=49; 


		
	  if(fDif_id==80) DifX=64, DifZ=48; 
		if(fDif_id==182) DifX=32, DifZ=48; 
		if(fDif_id==105) DifX=0,  DifZ=48; 
	       	





		
		//		std::cout << "event number ="<< event_count << std::endl;
		//		nhit_count++;
		//	if(nhit_count>100000)
		//	{
		//		std::cout << "here we come"<<std::endl;
		// return;	
		//	}

     fY=fY+DifX*1.04125;
     //pour le prototype
     float fYt = 100-fX;
     fX=fY;
     fY=fYt; 
     //fin 

		 //		 		 if(fDif_id==78)std::cout << fY << "," << fX << "," << fZ << "," << fDif_id <<std::endl;     
					       fZ= DifZ*2.8; //real Z coordiante (cm)		    






								 // 28, 198, 32, 2, 23

								 if(fDif_id==28) 
            {


	
fX = (1+MapILargeHR2[fChan_id]+AsicShiftI[fAsic_id])*1.04125; // X coordinate (cm)
    
fY = (32-(MapJLargeHR2[fChan_id]+AsicShiftJ[fAsic_id]))*1.04125; //Y coordinate (cm)
							fZ=-130;
							 fX=fX+35;	 
             fY=fY+35; 
            }  

		 if(fDif_id==198) 
            {

	
fX = (1+MapILargeHR2[fChan_id]+AsicShiftI[fAsic_id])*1.04125; // X coordinate (cm)
    
fY = (32-(MapJLargeHR2[fChan_id]+AsicShiftJ[fAsic_id]))*1.04125; //Y coordinate (cm)
							fZ=-130+9;
							 fX=fX+35;	 
             fY=fY+35; 
            }  

		 if(fDif_id==32) 
            {

	
fX = (1+MapILargeHR2[fChan_id]+AsicShiftI[fAsic_id])*1.04125; // X coordinate (cm)
    
fY = (32-(MapJLargeHR2[fChan_id]+AsicShiftJ[fAsic_id]))*1.04125; //Y coordinate (cm)		
					fZ=-130+2*9;
							 fX=fX+35;	 
             fY=fY+35; 
            }  

		 if(fDif_id==2) 
            {

	
fX = (1+MapILargeHR2[fChan_id]+AsicShiftI[fAsic_id])*1.04125; // X coordinate (cm)
    
fY = (32-(MapJLargeHR2[fChan_id]+AsicShiftJ[fAsic_id]))*1.04125; //Y coordinate (cm)
							fZ=-130+3*9;
							 fX=fX+35;	 
             fY=fY+35; 
            }  


		 if(fDif_id==23) 
            {


	
fX = (1+MapILargeHR2[fChan_id]+AsicShiftI[fAsic_id])*1.04125; // X coordinate (cm)
    
fY = (32-(MapJLargeHR2[fChan_id]+AsicShiftJ[fAsic_id]))*1.04125; //Y coordinate (cm)
							fZ=-130+4*9;
							 fX=fX+35;	 
             fY=fY+35; 
            }  





		 // if(DifZ>=33)       fZ= DifZ*10; //real Z coordiante (cm)		    
		 // if(fZ>8.&&fZ<10.)
		 //	 {
							 
		 // }
    fTime=a_hit.getTimeStamp(); 
   
    //thresholds settings
	   int ithr= (a_hit.getAmplitude()&0x3);
		//  int ithr= (a_hit.getAmplitude());
    if (ithr < 1) 	std::cout << "seuil" << ithr <<std::endl;       
    if (ithr == 2) fThr0 =1; // first threshold (the lowest)
    if (ithr == 1) fThr1 =1; // second threshold
    if (ithr == 3)  fThr2 =1; // third threshold
    //		std::cout << "Time" << fTime*2./10000.<<std::endl;       

			}
  };

	  
  
  
 

  Double_t GetTime() const      {return fTime;};
  void SetTime(Double_t time)   {fTime=time;};
  
  Double_t fX;
  Double_t fY;
  Double_t fZ;
  unsigned int fDif_id;
  int fAsic_id;
  int fChan_id;
 
  Double_t fTime;
  Int_t fThr0,fThr1,fThr2;
  
    };
#endif
