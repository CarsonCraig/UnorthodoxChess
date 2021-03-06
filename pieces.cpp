#include "pieces.h"

GLuint texture[32];
float angle = 0.0f;
GLuint p,f,v,tile;
int grid_row, grid_col;
char grid_column;
int highlighted_tiles[56][2] = {0};
int grid_pieces[8][8] = {0}; 
GAMESTATE gamestate;
char column[9] = {'a','b','c','d','e','f','g','h','z'};
std::vector<Piece*> pieces,side_pieces;
int blackLeftRook, blackRightRook, whiteLeftRook, whiteRightRook;
int blackKing, whiteKing;

void setBlackLeftRook(int value){
    blackLeftRook = value;
}
void setBlackRightRook(int value){
    blackRightRook = value;
}
void setWhiteLeftRook(int value){
    whiteLeftRook = value;
}
void setWhiteRightRook(int value){
    whiteRightRook = value;
}
void setBlackKing(int value){
    blackKing = value;
}
void setWhiteKing(int value){
    whiteKing = value;
}

Piece* piece_at(int col, int row){
    for(int i = 0; i <= pieces.size()-1; i++){
        if(pieces.at(i)->c_Row == row && pieces.at(i)->c_Col == col){
            return pieces.at(i);
        }
    }
}

void remove_piece(int col, int row){
    Piece* temp = piece_at(col, row);
    auto it = std::find(pieces.begin(), pieces.end(), temp);
    side_pieces.push_back(temp);
    if(it != pieces.end()){
        pieces.erase(it);
        grid_pieces[row-1][col-1] = 0;
    }
}

void initDLs(void){
    printf("%i\n",A);
    tile = glGenLists(1);
    glNewList(tile,GL_COMPILE);
    glBegin(GL_QUADS);
    glVertex3f(1,0,1);
    glVertex3f(-1,0,1);
    glVertex3f(-1,0,-1);
    glVertex3f(1,0,-1);
    glEnd();
    glEndList();
}

void drawGrid(void){
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glColor4f(1,1,1,0);
    int counter = 0;
    for(int i = 0; i <= 7; i++){
        for(int j = 0; j <= 7; j++){
            counter++;
            glLoadName(100+counter);
            glPushMatrix();
            glColor4f(1,1,1,0);
            glTranslatef(-7+(i*2),-0.97,(-7+(j*2)));
            glCallList(tile);
            glPopMatrix();
        }
    }
    for(int k = 0; k < sizeof(highlighted_tiles)/sizeof(highlighted_tiles[0]); k++){
        if(highlighted_tiles[k] != 0){
            int row = highlighted_tiles[k][0]-1;
            int col = highlighted_tiles[k][1]-1;
            if(col >= 0 && col >= 0 && col <= 7 && col <= 7){
                glLoadName(165);
                glPushMatrix();
                if(grid_pieces[row][col] == BLACK && gamestate == WHITE_TURN){
                    glColor4f(1,0,0,0.75);
                }
                else if(grid_pieces[row][col] == WHITE && gamestate == BLACK_TURN){
                    glColor4f(1,0,0,0.75);
                }
                else{
                    glColor4f(0,0,1,0.75);
                }
                
                glTranslatef(-7+(col*2),-0.96,-(-7+(row*2)));
                glCallList(tile);
                glBegin(GL_LINES);
                glEnd();
                glPopMatrix();
            }
        }
    }
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void setShaders(void){
    char *vs = NULL,*fs = NULL;

    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);

    vs = textFileRead("shaders/swag.vert");
    fs = textFileRead("shaders/swag.frag");

    const char *vv = vs;
    const char *ff = fs;

    glShaderSource(v,1,&vv,NULL);
    glShaderSource(f,1,&ff,NULL);

    free(vs);free(fs);

    glCompileShader(v);
    glCompileShader(f);

    p = glCreateProgram();
    
    glAttachShader(p,f);
    glAttachShader(p,v);

    glLinkProgram(p);
    glUseProgram(p);
}

Piece::Piece(const char* modelFile, const char* textureFile,int textureNum,char col,int row){
    angle = 0.0f;
    loadOBJ(modelFile,vertexArray,normalArray,uvArray,numVerts);
    textureNumber = textureNum;
    loadGLTexture(textureFile,textureNumber,texture);
    c_Row = row;
    for(unsigned int i = 0; i <= sizeof(column)/sizeof(char); i++){
        if(tolower(column[i]) == tolower(col)){
            c_Col = i+1; 
        }
    }
    c_Column = col;
    if(strstr(textureFile,"black")){
        color = BLACK;
        grid_pieces[c_Row-1][c_Col-1] = color;
    }else if(strstr(textureFile,"white")){
        color = WHITE;
        grid_pieces[c_Row-1][c_Col-1] = color;
    }else{
        color = -1;
    }
}

void Piece::render(void){
    glBindTexture(GL_TEXTURE_2D,texture[textureNumber]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3,GL_FLOAT,0,vertexArray);
    glNormalPointer(GL_FLOAT,0,normalArray);

    glTexCoordPointer(2,GL_FLOAT,0,uvArray);

    glDrawArrays(GL_TRIANGLES,0,numVerts);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Piece::draw(bool side_piece, float pX, float pZ){
    float x = pX, z = pZ;
    glPushMatrix();

    if(!side_piece){
        x = -11.3+(2.5*c_Col);
        z = 11.3-(2.5*c_Row);
    }
    if(c_Column != 'z'){ 
        glColor3f(1,1,1);
        glScalef(0.8,0.8,0.8);
        glTranslatef(x,-1.25,z);
    }else if(c_Column == 'z'){ 
        glColor4ub(238,221,187,255);
        glTranslatef(0,-2,0);
    }
    render();

    glPopMatrix();
}

void Knight::draw(bool side_piece, float pX, float pZ){
    float x = pX, z = pZ;
    glPushMatrix();

    if(!side_piece){
        x = -11.3+(2.5*c_Col);
        z = 11.3-(2.5*c_Row);
    }
    glColor3f(1,1,1);
    glScalef(0.8,0.8,0.8);
    glTranslatef(-7.3+x,-2.25,-0.8+z);
    render();

    glPopMatrix();
}

bool checkSquare(int col, int row){
    if(grid_pieces[row-1][col-1] >= 1){
        return true;
    }
    return false;
}

int get_index(int col, int row){
    auto it = std::find(pieces.begin(), pieces.end(), piece_at(col,row));
    if (it == pieces.end()){
        return -1;
    }else{
        return std::distance(pieces.begin(), it);
    }
}

void highlight_tile(int col, int row,unsigned int tile, bool captured_mode = false){
    if((col <= 8 && row <= 8 && col > 0 && row > 0) && ((gamestate == WHITE_TURN && grid_pieces[row-1][col-1] != WHITE) ||(gamestate == BLACK_TURN && grid_pieces[row-1][col-1] != BLACK))){
        highlighted_tiles[tile][0] = row;
        highlighted_tiles[tile][1] = col;
    }
}

void highlight_tile_k(int col, int row,unsigned int tile, bool captured_mode = false){
    highlight_tile(col, row, tile, captured_mode);
}

void Piece::pick(void){
    picked = true;
    listMoves();
    grid_row = c_Row;
    grid_col = c_Col;
    grid_column = c_Column;
}

void Piece::unpick(void){
    picked = false;
}

void clearMovesList(void){
    memset(highlighted_tiles,0,sizeof(highlighted_tiles[0][0])*(sizeof(highlighted_tiles)/sizeof(highlighted_tiles[0]))*2); 
}

void Piece::createMoveList(int col_inc, int row_inc, int min_array){ 
    int r = c_Row, c = c_Col;
    for(int i = 0; i < 7; i++){
        c = c_Col-(col_inc*(i+1));
        r = c_Row-(row_inc*(i+1));
        if(checkSquare(c,r)){ 
            if(!((grid_pieces[r-1][c-1] == BLACK && gamestate == BLACK_TURN) || (grid_pieces[r-1][c-1] == WHITE && gamestate == WHITE_TURN))){
                highlight_tile(c,r,min_array+i,true);
            }
            break;
        }
        highlight_tile(c,r,min_array+i);
    }
}

void Piece::listMoves(void){
    clearMovesList();
}

void Pawn::listMoves(void){
    clearMovesList();
    if(color == BLACK){
        if(!checkSquare(c_Col, c_Row-1)){ 
            highlight_tile(c_Col, c_Row-1, 0);
            if(firstMove && !checkSquare(c_Col, c_Row-2)) 
                highlight_tile(c_Col, c_Row-2, 1);
        }
        
        if(grid_pieces[c_Row-2][c_Col-2] == WHITE){
            highlight_tile(c_Col-1, c_Row-1, 2, true);
        }
        if(grid_pieces[c_Row-2][c_Col] == WHITE){
            highlight_tile(c_Col+1, c_Row-1, 3, true);
        }
        
        if(grid_pieces[c_Row-1][c_Col-2] == WHITE){
            if(piece_at(c_Col-1,c_Row)->value == 1 && piece_at(c_Col-1,c_Row)->en_passant){ 
                highlight_tile(c_Col-1, c_Row-1, 4, true);
            }
        }
        if(grid_pieces[c_Row-1][c_Col] == WHITE){
            if(piece_at(c_Col+1,c_Row)->value == 1 && piece_at(c_Col+1,c_Row)->en_passant){
                highlight_tile(c_Col+1, c_Row-1, 4, true);
            }
        }
    }else if(color == WHITE){
        if(!checkSquare(c_Col, c_Row+1)){
            highlight_tile(c_Col, c_Row+1, 0);
            if(firstMove && !checkSquare(c_Col, c_Row+2))
                highlight_tile(c_Col, c_Row+2, 1);
        }
        if(grid_pieces[c_Row][c_Col-2] == BLACK){
            highlight_tile(c_Col-1, c_Row+1, 2, true);
        }
        if(grid_pieces[c_Row][c_Col] == BLACK){
            highlight_tile(c_Col+1, c_Row+1, 3, true);
        }
        
        if(grid_pieces[c_Row-1][c_Col-2] == BLACK){
            if(piece_at(c_Col-1, c_Row)->value == 1 && piece_at(c_Col-1,c_Row)->en_passant){
                highlight_tile(c_Col-1, c_Row+1, 4, true);
            }
        }
        if(grid_pieces[c_Row-1][c_Col] == BLACK){
            if(piece_at(c_Col+1, c_Row)->value == 1 && piece_at(c_Col+1,c_Row)->en_passant){
                highlight_tile(c_Col+1, c_Row+1, 4, true);
            }
        }
    }
}

void Knight::listMoves(void){
    clearMovesList();
    highlight_tile(c_Col-1,c_Row+2,0);
    highlight_tile(c_Col+1,c_Row+2,1);
    highlight_tile(c_Col-1,c_Row-2,2);
    highlight_tile(c_Col+1,c_Row-2,3);
    highlight_tile(c_Col-2,c_Row+1,4);
    highlight_tile(c_Col-2,c_Row-1,5);
    highlight_tile(c_Col+2,c_Row+1,6);
    highlight_tile(c_Col+2,c_Row-1,7);
}

void Bishop::listMoves(void){
    clearMovesList();
    createMoveList(1,1,0);
    createMoveList(-1,1,7);
    createMoveList(-1,-1,14);
    createMoveList(1,-1,21);
}

void Rook::listMoves(void){
    clearMovesList();
    createMoveList(1,0,0);
    createMoveList(-1,0,7);
    createMoveList(0,1,14);
    createMoveList(0,-1,21);
}

void King::listMoves(void){
    clearMovesList();
    highlight_tile_k(c_Col+1,c_Row,0);
    highlight_tile_k(c_Col-1,c_Row,1);
    highlight_tile_k(c_Col,c_Row+1,2);
    highlight_tile_k(c_Col,c_Row-1,3);
    highlight_tile_k(c_Col+1,c_Row+1,4);
    highlight_tile_k(c_Col+1,c_Row-1,5);
    highlight_tile_k(c_Col-1,c_Row+1,6);
    highlight_tile_k(c_Col-1,c_Row-1,7);
}

void Queen::listMoves(void){
    clearMovesList();
    createMoveList(1,0,0);
    createMoveList(-1,0,7);
    createMoveList(0,1,14);
    createMoveList(0,-1,21);
    createMoveList(1,1,28);
    createMoveList(-1,1,35);
    createMoveList(-1,-1,42);
    createMoveList(1,-1,48);
}

void Pawn::check_promotion(void){
    Queen* temp = NULL;
    int index = get_index(c_Col,c_Row);
    if(index < 0)
        return;
    if(color == BLACK && c_Row == 1){
        temp = new Queen("data/models/queen.dae","data/textures/black_queen.jpg",3,c_Column,c_Row);
        pieces.at(index) = temp;
    }else if(color == WHITE && c_Row == 8){
        temp = new Queen("data/models/queen.dae","data/textures/white_queen.jpg",2,c_Column,c_Row);
        pieces.at(index) = temp;
    }
}

void Piece::move(unsigned int col, unsigned int row){
    grid_pieces[c_Row-1][c_Col-1] = 0;
    c_Col = col;
    c_Row = row;
    c_Column = column[col-1];
    grid_pieces[c_Row-1][c_Col-1] = color;
    has_moved = true;
}

void Pawn::move(unsigned int col, unsigned int row){
    Piece::move(col,row);
    if(color == WHITE){
        if(grid_pieces[row-2][col-1] == BLACK && piece_at(col,row-1)->en_passant){
            remove_piece(col,row-1);
        }
    }else if(color == BLACK){
        if(grid_pieces[row][col-1] == WHITE && piece_at(col,row+1)->en_passant){
            remove_piece(col,row+1);
        }
    }
    for(int i = 0; i <= pieces.size()-1; i ++){
        pieces.at(i)->en_passant = false;
    }
    if(firstMove){
        firstMove = false;
        en_passant = true;
        printf("1st move\n");
    }
    has_moved = true;
    check_promotion();
}

void Piece::move(char col, unsigned int row){
    grid_pieces[c_Row-1][c_Col-1] = 0;
    for(unsigned int i = 0; i <= sizeof(column)/sizeof(char); i++){
        if(tolower(column[i]) == tolower(col)){
            c_Col = i+1;
        }
    }
    c_Row = row;
    c_Column = col;
    grid_pieces[c_Row-1][c_Col-1] = color;
    for(int i = 0; i <= pieces.size()-1; i ++){
        pieces.at(i)->en_passant = false;
    }
    has_moved = true;
}

void castle(unsigned char key){
    if(gamestate == WHITE_TURN && (piece_at(whiteKing, 1) == nullptr || piece_at(whiteKing, 1) -> has_moved == true)){
        return;
    }
    if(gamestate == BLACK_TURN && (piece_at(blackKing, 8) == nullptr || piece_at(blackKing, 8) -> has_moved == true)){
        return;
    }

    bool leftCastle = true;
    bool rightCastle = true;

    if(gamestate == WHITE_TURN){

        for(int i = whiteLeftRook + 1; i < whiteKing; i++){
            if(checkSquare(i,1)){
                leftCastle = false;
            }
        }

        if(piece_at(whiteLeftRook, 1) == nullptr || piece_at(whiteLeftRook, 1) -> has_moved == true){
            leftCastle = false;
        }

        for(int i = whiteKing + 1; i < whiteRightRook; i++){
            if(checkSquare(i,1)){
                rightCastle = false;
            }
        }

        if(piece_at(whiteRightRook, 1) == nullptr || piece_at(whiteRightRook, 1) -> has_moved == true){
            rightCastle = false;
        }

    }
    else{

        for(int i = blackLeftRook + 1; i < blackKing; i++){
            if(checkSquare(i,8)){
                leftCastle = false;
            }
        }

        if(piece_at(blackLeftRook, 8) == nullptr || piece_at(blackLeftRook, 8) -> has_moved == true){
            leftCastle = false;
        }

        for(int i = blackKing + 1; i < blackRightRook; i++){
            if(checkSquare(i,8)){
                rightCastle = false;
            }
        }

        if(piece_at(blackRightRook, 8) == nullptr || piece_at(blackRightRook, 8) -> has_moved == true){
            rightCastle = false;
        }

    }

    


    if(gamestate == WHITE_TURN && key == GLUT_KEY_LEFT && leftCastle){
        if(whiteKing == 2){
            piece_at(whiteLeftRook,1)-> move((unsigned int)(whiteKing),(unsigned int)1);
            piece_at(whiteKing, 1) -> move((unsigned int)(whiteKing - 1),(unsigned int)1);
            grid_pieces[whiteKing-1][1-1] = WHITE;
        }
        else if(whiteLeftRook-whiteKing == -1){
            if(!checkSquare(whiteKing-2,1)){
                piece_at(whiteLeftRook,1)-> move((unsigned int)(whiteKing - 1),(unsigned int)1);
                piece_at(whiteKing, 1) -> move((unsigned int)(whiteKing - 2),(unsigned int)1);
            }
            else{
                return;
            }
        }
        else{
            piece_at(whiteLeftRook,1)-> move((unsigned int)(whiteKing - 1),(unsigned int)1);
            piece_at(whiteKing, 1) -> move((unsigned int)(whiteKing - 2),(unsigned int)1);
        }
    }
    else if(gamestate == WHITE_TURN && key == GLUT_KEY_RIGHT && rightCastle){
        if(whiteKing == 7){
            piece_at(whiteRightRook,1)-> move((unsigned int)(whiteKing),(unsigned int)1);
            piece_at(whiteKing, 1) -> move((unsigned int)(whiteKing + 1),(unsigned int)1);    
            grid_pieces[whiteKing-1][1-1] = WHITE;        
        }
        else if(whiteRightRook-whiteKing == 1){
            if(!checkSquare(whiteKing+2,1)){
                piece_at(whiteRightRook,1)-> move((unsigned int)(whiteKing + 1),(unsigned int)1);
                piece_at(whiteKing, 1) -> move((unsigned int)(whiteKing + 2),(unsigned int)1);
            }
            else{
                return;
            }
        }
        else{
            piece_at(whiteRightRook,1)-> move((unsigned int)(whiteKing + 1),(unsigned int)1);
            piece_at(whiteKing, 1) -> move((unsigned int)(whiteKing + 2),(unsigned int)1);
        }
    }
    else if(gamestate == BLACK_TURN && key == GLUT_KEY_LEFT && leftCastle){
        if(blackKing == 2){
            piece_at(blackLeftRook,8)-> move((unsigned int)(blackKing),(unsigned int)8);
            piece_at(blackKing, 8) -> move((unsigned int)(blackKing - 1),(unsigned int)8);    
            grid_pieces[blackKing-1][8-1] = BLACK;        
        }
        else if(blackLeftRook-blackKing == -1){
            if(!checkSquare(blackKing-2,8)){
                piece_at(blackLeftRook,8)-> move((unsigned int)(blackKing - 1),(unsigned int)8);
                piece_at(blackKing, 8) -> move((unsigned int)(blackKing - 2),(unsigned int)8);
            }
            else{
                return;
            }
        }
        else{
            piece_at(blackLeftRook,8)-> move((unsigned int)(blackKing - 1),(unsigned int)8);
            piece_at(blackKing, 8) -> move((unsigned int)(blackKing - 2),(unsigned int)8);
        }
    }
    else if(gamestate == BLACK_TURN && key == GLUT_KEY_RIGHT && rightCastle){
        if(blackKing == 7){
            piece_at(blackRightRook,8)-> move((unsigned int)(blackKing),(unsigned int)8);
            piece_at(blackKing, 8) -> move((unsigned int)(blackKing + 1),(unsigned int)8);   
            grid_pieces[blackKing-1][8-1] = BLACK;            
        }
        else if(blackRightRook-blackKing == 1){
            if(!checkSquare(blackKing+2,8)){
                piece_at(blackRightRook,8)-> move((unsigned int)(blackKing + 1),(unsigned int)8);
                piece_at(blackKing, 8) -> move((unsigned int)(blackKing + 2),(unsigned int)8);   
            }
            else{
                return;
            }
        }
        else{
            piece_at(blackRightRook,8)-> move((unsigned int)(blackKing + 1),(unsigned int)8);
            piece_at(blackKing, 8) -> move((unsigned int)(blackKing + 2),(unsigned int)8);   
        }
    }
    else{
        return;
    }

    clearMovesList();
    if(gamestate == WHITE_TURN){
        gamestate = BLACK_TURN;
    }
    else{
        gamestate = WHITE_TURN;
    }
}



