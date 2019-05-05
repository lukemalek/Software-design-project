#include "Neural_net.h"

    bool isEmpty[7][6];
    bool isRed[7][6];
    bool isBlack[7][6];
  bool availableSelections[7];
  //bool[42][]
    int open;
    bool isGameOver;
    bool blacksMove;
  bool isDraw;
  bool blackWon;
  int moveNumber;
  vector<float> inputStorage[42];
  float outputStorage [42][7];
  int choiceStorage[42];
    void initializeGameBoard(){
      open =42;
      moveNumber = 0;
      isGameOver= false;
      blacksMove = true;
      isDraw = false;
      blackWon = NULL;
      for(int i = 0; i< 7; i++)
	availableSelections[i]=true;
        for(int i=0; i<7;i++){
            for(int j=0; j<6;j++){
                isEmpty[i][j] = true;
                isRed[i][j]=false;
                isBlack[i][j]=false;
            }
        }
    }


void makeMove(int c, bool blacksMove){
  int height = 5;
  while (isEmpty[c][height-1])
    height--;
  if(height == 5)
    availableSelections[c] = false;
  if(blacksMove)
    isBlack[c][height] = true;
  else
    isRed[c][height] = true;
  isEmpty[c][height] = false;
  open--;

  bool tmp [7][6];
  for(int i =0; i<7 ;i++){
      for(int j = 0; j<6;j++){
  if(blacksMove)
    tmp[i][j] = isBlack[i][j];
  else
    tmp[i][j] = isRed[i][j];
      }
  }

  int piecesInARow;
  int checkIncrement;
  bool possible;
  for(int i = 0; i < 7 ; i++){
    for (int j = 0; j < 6 ; j++){
      if(tmp[i][j] && isGameOver == false){
	piecesInARow = 1;
	possible = true;
	checkIncrement = 1;
	while(piecesInARow <4 && possible == true){
	if(i+checkIncrement<7 && j+checkIncrement<6){
	  if(tmp[i+checkIncrement][j+checkIncrement]){
	    piecesInARow++;
	    checkIncrement++;
	  }
	  else
	    possible = false;
	}else
	  possible = false;
	}
	if(possible==false){
	  piecesInARow = 1;
	  possible =true;
	  checkIncrement=1;
	  while(piecesInARow <4 && possible == true){
	    if(i+checkIncrement <7){
	      if(tmp[i+checkIncrement][j]){
		piecesInARow++;
		checkIncrement++;
	      }else
		possible = false;
	    }else
	      possible =false;
	  }
	}
	if(possible ==false){
	  piecesInARow =1;
	  possible = true;
	  checkIncrement =1;
	  while(piecesInARow < 4 && possible ==true){
	    if(j+checkIncrement<6){
	      if(tmp[i][j+checkIncrement]){
		piecesInARow++;
		checkIncrement++;
	      }else
		possible = false;
	    }else
	      possible = false;
	  }
	}
	if(piecesInARow>=4){
	  isGameOver = true;
	  isDraw = false;
	  if(blacksMove)
	    blackWon = true;
	  else
	    blackWon = false;
	}
      }
      
    }    
  }
  if(open == 0 && isGameOver ==false){
    isGameOver = true;
    isDraw = true;
  }  
    
}

void makeTurn(bool& blacksMove, Network& learner, Network& learner2){
        vector<float> inputs;
        float outputs [7];
        for(int i = 0; i < 7; i++){
            for(int j = 0; j<6; j++){
            inputs.push_back(isRed[i][j]);
            inputs.push_back(isBlack[i][j]);
            inputs.push_back(isEmpty[i][j]);
            }
        }

    inputStorage [moveNumber] = inputs;

    if(blacksMove){
        learner.setInputLayer(inputs);
        learner.evaluate();
        for(int i=0; i<7; i++){
            outputs [i] = learner.getOutput(i);
        }
    } 
    else
    {
        learner2.setInputLayer(inputs);
        learner2.evaluate();
        for(int i=0; i<7; i++){
            outputs [i] = learner2.getOutput(i);
        }
    }
    int choice = -1;
    double value = -1000;
    for(int i = 0; i<7; i++){
        if(availableSelections[i]){
            if(outputs[i]>= value){
                choice = i;
                value = outputs[i];
            }
        }

    }

    for (int i = 0; i<7; i++){
        outputStorage[moveNumber][i] = outputs[i];
    }
    choiceStorage[moveNumber] = choice;

    makeMove(choice,blacksMove);
    //cout<<"got here";
        if(blacksMove == true){
        blacksMove=false;
    }
        else
        {
            blacksMove = true;
        } 
    //cout<<" and here";   
}

int main()
{
    srand(time(NULL));
    const vector<int> dimensions = {7*6*3,20, 7};
    int firstLayer = dimensions[0];

    //bool isRedsTurn = TRUE;

    Network learner("connect4black.net");
    Network learner2("connect4red.net");

for(int numGames=0; numGames< 500; numGames++){
    initializeGameBoard();
    while(!isGameOver){
        makeTurn(blacksMove, learner, learner2);
        moveNumber++;
        //cout<<"turn made, open is now "<<open<<" blacks move is "<<blacksMove<<endl;
    }

    vector <float> wanted(7);
    Network temp(dimensions, false);
    Network temp2(dimensions, false);
    double totalCost = 0;

    for(int index = 0; index < moveNumber; index++){
        if(index%2 == 0){
            //stuff for the learner 1 network which handles black moves
            if(blackWon){
                for(int i = 0; i<7; i++){
                    if(i == choiceStorage[index])
                    wanted[i]=1;
                    else
                    wanted[i]=0;
                }
            }else{
                for(int i = 0; i<7; i++){
                    if(i == choiceStorage[index])
                    wanted[i]=0;
                    else
                    wanted[i]=1;
                }
            }
            learner.setInputLayer(inputStorage[index]);
            learner.evaluate();
            temp += learner.gradient(wanted);
            totalCost+= learner.cost(wanted);

        }else{
            //stuff for the learner 2 network which handles red moves
            if(!blackWon){
                for(int i = 0; i<7; i++){
                    if(i == choiceStorage[index])
                    wanted[i]=1;
                    else
                    wanted[i]=0;
                }
            }else{
                for(int i = 0; i<7; i++){
                    if(i == choiceStorage[index])
                    wanted[i]=0;
                    else
                    wanted[i]=1;
                }
            }

            learner2.setInputLayer(inputStorage[index]);
            learner2.evaluate();
            
            
            temp2 += learner2.gradient(wanted);
            totalCost+= learner2.cost(wanted);
    }
    }
    learner-= temp;
    learner2 -= temp2;
    cout<<"Cost per move: "<<totalCost/moveNumber<<endl;
}
    learner.toFile("connect4black.net");
    learner2.toFile("connect4red.net");
//for(int index =0; index<(42-moves);i++){

//}
    //cout<<open<<endl;

    /*
    int subsetSize = 10, rate = 10;

    for(int i = 0; i< 500; i++)
    {
        float boop = 0;
        Network temp(dimentions, false);
        for(int j = 0; j<subsetSize; j++)
        {
            vector<float> input, wanted(2);
            if(rnum() > 0.5)
            {
                int spot = (int)(7720 * rnum());
                input = formToInput(engData[spot],firstLayer, true);
                wanted[0] =1;
                wanted[1] = 0;
            }
            else
            {
                int spot = (int)(5334 * rnum());
                input = formToInput(gibData[spot],firstLayer,true);
                wanted[0] = 0;
                wanted[1] = 1;
            }
            learner.setInputLayer(input);
            learner.evaluate();
            
            
            temp += learner.gradient(wanted);
            boop += learner.cost(wanted);
        }
        temp /= (subsetSize / (float)rate);
        learner-= temp;
        cout<< boop/subsetSize << endl;
    }
    learner.toFile("englishUpper.net");
    

*/
    
}