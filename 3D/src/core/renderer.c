#include "renderer.h"
#include "../../includes/raymath.h"
#include "../utils/utils.h"
#include "../../includes/rlgl.h"
#define DEBUG_INFO_LINE_COUNT 5
drawBundle_t drawBundle = {0, 0, 1, {0, 0, 0}, {0, 0, 0}, 0, 0, 0};


Model keyModel;
Model potionModel;
Model cubeModel;
Model doorModel;
Model monsterModel;
Model powerUpAttackModel;
Model powerUpShieldModel;
Model powerUpHealthModel;

Texture2D keyETexture;


Texture2D heartFullTexture;
Texture2D heartEmptyTexture;
Texture2D keycount;
Texture2D overlayBackgroundTexture;
Texture2D bigHeartTexture;
Texture2D bigArmorTexture;
Texture2D bigSwordTexture;
Texture2D wingedHeartTexture;
Texture2D wallTexture;
Texture2D floorTexture;
Texture2D crackedWallTexture;
Texture2D doorUpTexture;
Texture2D doorDownTexture;
Texture2D potionTexture;
Texture2D swordTexture;
Texture2D armorTexture;
Texture2D fightDialogBackground;

void initRenderer(player_t * player) {
    drawBundle.player = player;
    monsterModel = LoadModel("./assets/monster2.glTF");
    keyModel = LoadModel("./assets/key.obj");
    doorModel = LoadModel("./assets/door.obj");
    potionModel = LoadModel("./assets/potion1.obj");
    heartFullTexture = LoadTexture("./assets/heart.png");
    heartEmptyTexture = LoadTexture("./assets/heart_empty.png");
    armorTexture = LoadTexture("./assets/armor.png");
    swordTexture = LoadTexture("./assets/sword.png");
    keycount = LoadTexture("./assets/keycount.png");
    overlayBackgroundTexture = LoadTexture("./assets/background.png");
    bigHeartTexture = LoadTexture("./assets/bigheart.png");
    bigArmorTexture = LoadTexture("./assets/bigarmor.png");
    bigSwordTexture = LoadTexture("./assets/bigsword.png");
    wingedHeartTexture = LoadTexture("./assets/wingsheart.png");
    wallTexture = LoadTexture("./assets/mossy_stone_bricks.png");
    floorTexture = LoadTexture("./assets/stone_bricks.png");
    crackedWallTexture = LoadTexture("./assets/cracked_stone.png");
    doorUpTexture = LoadTexture("./assets/door_up.png");
    doorDownTexture = LoadTexture("./assets/door_down.png");
    powerUpAttackModel = LoadModel("./assets/sword.obj");
    powerUpShieldModel = LoadModel("./assets/shield.obj");
    powerUpHealthModel = LoadModel("./assets/love_heart.glTF");
    fightDialogBackground = LoadTexture("./assets/dialog.png");

    keyETexture = LoadTexture("./assets/key_e.png");



    //potion = LoadTexture("./assets/potion.png"); 
    


}



Texture2D getWallTexture(int type) {
    switch (type) {
        case 1:
            return floorTexture;
        case 2:
            return wallTexture;
        case 3 :
            return crackedWallTexture;
    }
    return floorTexture;
}



void DrawMap(chunkedMap_t map) {
    for (int i = 0; i < map.width; i++) {
        for (int j = 0; j < map.height; j++) {
            if (map.chunks[i][j].x != -1) {
                DrawChunk(map.chunks[i][j]);
            }
        }
    }
}

//TODO Add DrawCeilling & DrawGround
void DrawChunk(chunk_t chunk) {
    //GROUND
    for(int x = chunk.x * CHUNK_SIZE; x < (chunk.x + 1) * CHUNK_SIZE; x++) {
        for(int y = 0; y < MAX_Y; y++) {
            for(int z = chunk.y * CHUNK_SIZE; z < (chunk.y + 1) * CHUNK_SIZE; z++) {
                if(y == 0){
                    DrawCubeCustom(floorTexture, (Vector3){x + 0.5, -0.5, z + 0.5}, 1.0f, 1.0f, 1.0f, WHITE);
                    continue;
                }
                if(y==5 && drawBundle.drawCeiling){
                    DrawCubeCustom(floorTexture, (Vector3){x + 0.5, 4.5, z + 0.5}, 1.0f, 1.0f, 1.0f, WHITE);
                    
                }
            
                if(chunk.chunk[x%CHUNK_SIZE][y][z%CHUNK_SIZE]!=0) {
                    DrawCubeCustom(getWallTexture(chunk.chunk[x % CHUNK_SIZE][y][z % CHUNK_SIZE]),
                                   (Vector3) {x + 0.5, y - 0.5, z + 0.5}, 1.0f, 1.0f, 1.0f, WHITE);
                    //DrawCubeWires((Vector3) {x + 0.5, y- 0.5, z + 0.5}, 1.0f, 1.0f, 1.0f, MAROON);
                }
            }
        }
    }
    for(int i = 0; i < chunk.doorCount; i++) {
        DrawDoor(chunk.doors[i], chunk.x, chunk.y);
    }
    for(int i = 0; i < chunk.potionCount; i++) {
        DrawPotion(chunk.potions[i], chunk.x, chunk.y);
    }
    for(int i = 0; i < chunk.potionCount; i++) {
        DrawPotion(chunk.potions[i],chunk.x,chunk.y);
    }
    for(int i = 0; i < chunk.keyCount; i++) {
        DrawKey(chunk.keys[i], chunk.x, chunk.y);
    }
    for(int i = 0; i < chunk.monsterCount; i++) {
        DrawMonster(chunk.monsters[i], chunk.x, chunk.y);
    }
    for(int i = 0; i < chunk.powerUpCount; i++) {
        DrawPowerUp(chunk.powerUps[i], chunk.x, chunk.y);
    }
}

void DrawCubeCustom(Texture2D texture, Vector3 position, float width, float height, float length, Color color){
    float x = position.x;
    float y = position.y;
    float z = position.z;

    rlSetTexture(texture.id);

    rlBegin(RL_QUADS);
    rlColor4ub(color.r, color.g, color.b, color.a);

    // NOTE: Enable texture 1 for Front, Back
    rlEnableTexture(texture.id);

    // Front Face
    // Normal Pointing Towards Viewer
    rlNormal3f(0.0f, 0.0f, 1.0f);

    // Bottom Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(x - width / 2, y - height / 2, z + length / 2);

    // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f);
    rlVertex3f(x + width / 2, y - height / 2, z + length / 2);

    // Top Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f);
    rlVertex3f(x + width / 2, y + height / 2, z + length / 2);

    // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f);
    rlVertex3f(x - width / 2, y + height / 2, z + length / 2);

    // Back Face
    // Normal Pointing Away From Viewer
    rlNormal3f(0.0f, 0.0f, -1.0f);

    // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f);
    rlVertex3f(x - width / 2, y - height / 2, z - length / 2);

    // Top Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f);
    rlVertex3f(x - width / 2, y + height / 2, z - length / 2);

    // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f);
    rlVertex3f(x + width / 2, y + height / 2, z - length / 2);

    // Bottom Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(x + width / 2, y - height / 2, z - length / 2);

    // NOTE: Enable texture 2 for Top, Bottom, Left and Right
    rlEnableTexture(texture.id);

    // Top Face
    // Normal Pointing Up
    rlNormal3f(0.0f, 1.0f, 0.0f);

    // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f);
    rlVertex3f(x - width / 2, y + height / 2, z - length / 2);

    // Bottom Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(x - width / 2, y + height / 2, z + length / 2);

    // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f);
    rlVertex3f(x + width / 2, y + height / 2, z + length / 2);

    // Top Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f);
    rlVertex3f(x + width / 2, y + height / 2, z - length / 2);

    // Bottom Face
    // Normal Pointing Down
    rlNormal3f(0.0f, -1.0f, 0.0f);

    // Top Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f);
    rlVertex3f(x - width / 2, y - height / 2, z - length / 2);

    // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f);
    rlVertex3f(x + width / 2, y - height / 2, z - length / 2);

    // Bottom Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(x + width / 2, y - height / 2, z + length / 2);

    // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f);
    rlVertex3f(x - width / 2, y - height / 2, z + length / 2);

    // Right face
    // Normal Pointing Right
    rlNormal3f(1.0f, 0.0f, 0.0f);

    // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f);
    rlVertex3f(x + width / 2, y - height / 2, z - length / 2);

    // Top Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f);
    rlVertex3f(x + width / 2, y + height / 2, z - length / 2);

    // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f);
    rlVertex3f(x + width / 2, y + height / 2, z + length / 2);

    // Bottom Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(x + width / 2, y - height / 2, z + length / 2);

    // Left Face
    // Normal Pointing Left
    rlNormal3f(-1.0f, 0.0f, 0.0f);

    // Bottom Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(x - width / 2, y - height / 2, z - length / 2);

    // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f);
    rlVertex3f(x - width / 2, y - height / 2, z + length / 2);

    // Top Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f);
    rlVertex3f(x - width / 2, y + height / 2, z + length / 2);

    // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f);
    rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
    rlEnd();

    rlDisableTexture();
}


// gerer le max health et faire un design
// quand triche faire un design armor et epee different
void DrawOverlay() {
    int bigHeart = 0;
    int smallheart = drawBundle.player->statistics.health;
    int bigArmor = 0;
    int smallarmor = drawBundle.player->statistics.armor;
    int bigDamage = 0;
    int smalldamage = drawBundle.player->statistics.damage;


    if(smallheart % 10 == 0){
        smallheart = 10;
        bigHeart = (drawBundle.player->statistics.health / 10)-1;
    }
    else {
        smallheart = smallheart % 10;
        bigHeart = drawBundle.player->statistics.health / 10;
    }
    if(smallarmor % 10 == 0){
        smallarmor = 10;
        bigArmor = (drawBundle.player->statistics.armor/ 10)-1;
    }
    else {
        smallarmor = smallarmor % 10;
        bigArmor = drawBundle.player->statistics.armor / 10;
    }
    if(smalldamage % 10 == 0){
        smalldamage = 10;
        bigDamage = (drawBundle.player->statistics.damage/ 10)-1;
    }
    else {
        smalldamage = smalldamage % 10;
        bigDamage = drawBundle.player->statistics.damage / 10;
    }


     if(drawBundle.drawOverlay) {
        int i = 0;

        DrawTexture(overlayBackgroundTexture, 0, 0, WHITE);
        for(i = 0; i < 10; i++) {
            DrawTexture(heartEmptyTexture, 7 + 36 * i, 15, WHITE);
        }
        for(i = 0; i < smallheart; i++) {
            DrawTexture(heartFullTexture, 7 + 36 * i, 15, WHITE);
        }
        

        if( drawBundle.player->statistics.health == drawBundle.player->statistics.maxHealth){
                DrawTexture(wingedHeartTexture, 430, 13, WHITE);
                
        }
        else {
                DrawTexture(bigHeartTexture,400, 13, WHITE);
                DrawText(TextFormat("x %d", bigHeart), 435,20, 15,WHITE);
            }
          
        for(i = 0; i < smallarmor; i++) {
            DrawTexture(armorTexture, 5 + 36* i, 48, WHITE);
        }
        for(i = 0; i < smalldamage; i++) {
            DrawTexture(swordTexture, 2 + 36 * i, 86, WHITE);
        }
        DrawTexture(keycount, 150, 133, WHITE);
        DrawText(TextFormat("x %d", drawBundle.player->keyCount),190,140, 15, WHITE);

        DrawTexture(bigArmorTexture,400, 47, WHITE);
        DrawText(TextFormat("x %d", bigArmor), 435 ,55, 15,WHITE);

        DrawTexture(bigSwordTexture,400, 87, WHITE);
        DrawText(TextFormat("x %d", bigDamage), 435 ,92, 15,WHITE);

     }
}

void DrawDebug() {
    if(drawBundle.drawDebug) {

        const char * infos[DEBUG_INFO_LINE_COUNT] = {
                TextFormat("cx: %d, cy: %d", ((int)drawBundle.player->camera->position.x) / CHUNK_SIZE, ((int)drawBundle.player->camera->position.z) / CHUNK_SIZE),
                TextFormat("x: %f, y: %f, z: %f", drawBundle.player->camera->position.x, drawBundle.player->camera->position.y, drawBundle.player->camera->position.z),
                TextFormat("looking at: (%f, %f, %f)", drawBundle.player->camera->target.x, drawBundle.player->camera->target.y, drawBundle.player->camera->target.z),
                TextFormat("move: (%f, %f, %f)", drawBundle.movement.x, drawBundle.movement.y, drawBundle.movement.z),
                TextFormat("rot: (%f, %f, %f)", drawBundle.direction.x, drawBundle.direction.y, drawBundle.direction.z)
        };

        for(int i = 0; i < DEBUG_INFO_LINE_COUNT; i++) {
            DrawText(infos[i], 10, 25 + 15 * i, 10, WHITE);
        }
    }
}
void Render(chunkedMap_t map) {
        ClearBackground(BLACK);
        BeginMode3D(*drawBundle.player->camera);

        DrawMap(map);

        EndMode3D();
        DrawDoorHint();
        DrawFightHint();
        DrawOverlay();
        DrawDebug();
        DrawFightDialog();

}

void DrawDoorHint() {
    if(drawBundle.canOpenDoor == 1) {
        DrawText("Press E to open door", 600, 800, 40, WHITE);
    }
    else if(drawBundle.canOpenDoor == -1) {
        DrawText("You need a key to open this door", 600, 800, 40, WHITE);
    }
}

void DrawFightHint() {
    if(drawBundle.canOpenFight == 1) {
        DrawText("Press E to attack", 600, 800, 40, WHITE);
    }
}
drawBundle_t getDrawBundle() {
    return drawBundle;
}


void DrawFightDialog() {
    if(drawBundle.player->inFight) {
        float dialogMarginX = WINDOW_WIDTH/2 - fightDialogBackground.width/2;
        float dialogMarginY = (WINDOW_HEIGHT - fightDialogBackground.height) * 9/10;

        DrawTexture(fightDialogBackground, dialogMarginX, dialogMarginY, WHITE);
        DrawText(drawBundle.dialog.text, dialogMarginX + 50, dialogMarginY + 50, 20, WHITE);

        DrawTexture(getKeyTexture(drawBundle.dialog.keys[1]), dialogMarginX + 20, dialogMarginY + fightDialogBackground.height * 8/10, WHITE);
        DrawText(drawBundle.dialog.choices[0], dialogMarginX + 50, dialogMarginY + fightDialogBackground.height * 8/10, 15, WHITE);

        if(drawBundle.dialog.choiceCount == 2) {
            DrawText(drawBundle.dialog.choices[1], dialogMarginX + 400, dialogMarginY + fightDialogBackground.height * 8/10, 20, WHITE);
        }
    }
}



void setDrawBundle(drawBundle_t bundle) {
    drawBundle = bundle;
}

Texture2D getKeyTexture(KeyboardKey key) {
    switch (key) {
        case KEY_E:
            return keyETexture;
        default:
            return keyETexture;
    }
}



#define RAYLIB_NEW_RLGL

// Draw a codepoint in 3D space
static void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, Color tint);
// Draw a 2D text in 3D space
static void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, Color tint);
// Measure a text works but the text is not drin 3D. For some reason `MeasureTextEx()` just doesn't seem to work so i had to use this instead.
static Vector3 MeasureText3D(Font font, const char *text, float fontSize, float fontSpacing, float lineSpacing);

int render3DText(const char * text, Vector3 position,float fontSize)
{
    Vector3 textDimension;
    // Initialization
    //--------------------------------------------------------------------------------------
    Font font = GetFontDefault();

    textDimension = MeasureText3D(GetFontDefault(), text, fontSize, 1.0f, 0.0f);

    rlPushMatrix();

    Vector3 direction = Vector3Normalize(Vector3Subtract(drawBundle.player->camera->position, position));
    float angleBetweenVectors = atan2f(direction.x, direction.z) * 180.0f / PI;
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    rlRotatef(angleBetweenVectors, 0.0f, 0.0f, -1.0f);

    DrawText3D(GetFontDefault(), text, (Vector3){-textDimension.x/2, 0, 0}, fontSize, 2.0f, 0.0f, RED);
    rlPopMatrix();

    UnloadFont(font);
    return 0;
}

//--------------------------------------------------------------------------------------
// Module Functions Definitions
//--------------------------------------------------------------------------------------raylib
// Draw codepoint at specified position in 3D space
static void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, Color tint)
{
    // Character index position in sprite font
    // NOTE: In case a codepoint is not available in the font, index returned points to '?'
    int index = GetGlyphIndex(font, codepoint);
    float scale = fontSize/(float)font.baseSize;

    // Character destination rectangle on screen
    // NOTE: We consider charsPadding on drawing
    position.x += (float)(font.glyphs[index].offsetX - font.glyphPadding)/(float)font.baseSize*scale;
    position.z += (float)(font.glyphs[index].offsetY - font.glyphPadding)/(float)font.baseSize*scale;

    // Character source rectangle from font texture atlas
    // NOTE: We consider chars padding when drawing, it could be required for outline/glow shader effects
    Rectangle srcRec = { font.recs[index].x - (float)font.glyphPadding, font.recs[index].y - (float)font.glyphPadding,
                         font.recs[index].width + 2.0f*font.glyphPadding, font.recs[index].height + 2.0f*font.glyphPadding };

    float width = (float)(font.recs[index].width + 2.0f*font.glyphPadding)/(float)font.baseSize*scale;
    float height = (float)(font.recs[index].height + 2.0f*font.glyphPadding)/(float)font.baseSize*scale;

    if (font.texture.id > 0)
    {
        const float x = 0.0f;
        const float y = 0.0f;
        const float z = 0.0f;

        // normalized texture coordinates of the glyph inside the font texture (0.0f -> 1.0f)
        const float tx = srcRec.x/font.texture.width;
        const float ty = srcRec.y/font.texture.height;
        const float tw = (srcRec.x+srcRec.width)/font.texture.width;
        const float th = (srcRec.y+srcRec.height)/font.texture.height;


        rlCheckRenderBatchLimit(8);
        rlSetTexture(font.texture.id);

        rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);

        rlBegin(RL_QUADS);
        rlColor4ub(tint.r, tint.g, tint.b, tint.a);

        // Front Face
        //rlNormal3f(0.0f, 1.0f, 0.0f);                                   // Normal Pointing Up
        rlTexCoord2f(tx, ty); rlVertex3f(x,         y, z);              // Top Left Of The Texture and Quad
        rlTexCoord2f(tx, th); rlVertex3f(x,         y, z + height);     // Bottom Left Of The Texture and Quad
        rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height);     // Bottom Right Of The Texture and Quad
        rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);              // Top Right Of The Texture and Quad

        //rlNormal3f(17.0f, 1.0f, 0.0f);                                   // Normal Pointing Up
        rlTexCoord2f(tx, ty); rlVertex3f(x,         y, z);          // Top Right Of The Texture and Quad
        rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);          // Top Left Of The Texture and Quad
        rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height); // Bottom Left Of The Texture and Quad
        rlTexCoord2f(tx, th); rlVertex3f(x,         y, z + height);

       // rlPushMatrix();

        rlEnd();
        rlPopMatrix();

        rlSetTexture(0);
    }
}

// Draw a 2D text in 3D space
static void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, Color tint)
{
    int length = TextLength(text);          // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0.0f;               // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;               // Offset X to next character to draw

    float scale = fontSize/(float)font.baseSize;

    for (int i = 0; i < length;)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;

        if (codepoint == '\n')
        {
            // NOTE: Fixed line spacing of 1.5 line-height
            // TODO: Support custom line spacing defined by user
            textOffsetY += scale + lineSpacing/(float)font.baseSize*scale;
            textOffsetX = 0.0f;
        }
        else
        {
            if ((codepoint != ' ') && (codepoint != '\t'))
            {
                DrawTextCodepoint3D(font, codepoint, (Vector3){ position.x + textOffsetX, position.y, position.z + textOffsetY }, fontSize, tint);
            }

            if (font.glyphs[index].advanceX == 0) textOffsetX += (float)(font.recs[index].width + fontSpacing)/(float)font.baseSize*scale;
            else textOffsetX += (float)(font.glyphs[index].advanceX + fontSpacing)/(float)font.baseSize*scale;
        }

        i += codepointByteCount;   // Move text bytes counter to next codepoint
    }
}

// Measure a text in 3D. For some reason `MeasureTextEx()` just doesn't seem to work so i had to use this instead.
static Vector3 MeasureText3D(Font font, const char* text, float fontSize, float fontSpacing, float lineSpacing)
{
    int len = TextLength(text);
    int tempLen = 0;                // Used to count longer text line num chars
    int lenCounter = 0;

    float tempTextWidth = 0.0f;     // Used to count longer text line width

    float scale = fontSize/(float)font.baseSize;
    float textHeight = scale;
    float textWidth = 0.0f;

    int letter = 0;                 // Current character
    int index = 0;                  // Index position in sprite font

    for (int i = 0; i < len; i++)
    {
        lenCounter++;

        int next = 0;
        letter = GetCodepoint(&text[i], &next);
        index = GetGlyphIndex(font, letter);

        // NOTE: normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set next = 1
        if (letter == 0x3f) next = 1;
        i += next - 1;

        if (letter != '\n')
        {
            if (font.glyphs[index].advanceX != 0) textWidth += (font.glyphs[index].advanceX+fontSpacing)/(float)font.baseSize*scale;
            else textWidth += (font.recs[index].width + font.glyphs[index].offsetX)/(float)font.baseSize*scale;
        }
        else
        {
            if (tempTextWidth < textWidth) tempTextWidth = textWidth;
            lenCounter = 0;
            textWidth = 0.0f;
            textHeight += scale + lineSpacing/(float)font.baseSize*scale;
        }

        if (tempLen < lenCounter) tempLen = lenCounter;
    }

    if (tempTextWidth < textWidth) tempTextWidth = textWidth;

    Vector3 vec = { 0 };
    vec.x = tempTextWidth + (float)((tempLen - 1)*fontSpacing/(float)font.baseSize*scale); // Adds chars spacing to measure
    vec.y = 0.25f;
    vec.z = textHeight;

    return vec;
}

void DrawSword(powerUp_t sword, int x, int y) {
    sword.position.y = sword.position.y + sin((float)GetTime() * 2) * 0.1f + 0.5f;
    sword.position.x += 0.5f + x * CHUNK_SIZE;
    sword.position.z += 0.5f + y * CHUNK_SIZE - 0.2;
    powerUpAttackModel.transform = MatrixRotateXYZ((Vector3){ 30 * DEG2RAD, 0, 0});
    DrawModel(powerUpAttackModel, sword.position, 1.0f, (Color){60,60,60,255});
   // potionModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture=potion;
}

void DrawShield(powerUp_t shield,int x,int y) {
    shield.position.y = shield.position.y + sin((float)GetTime() * 2) * 0.1f + 0.7f;
    shield.position.x += 0.5f + x * CHUNK_SIZE;
    shield.position.z += 0.5f + y * CHUNK_SIZE;
    powerUpShieldModel.transform = MatrixRotateXYZ((Vector3){ 0, 0, 0});
    DrawModel(powerUpShieldModel, shield.position, 0.3f, (Color){100, 100, 100, 200});
   // potionModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture=potion;
}
void DrawPowerUp(powerUp_t powerUp,int x, int y) {
    if(powerUp.pickedUp) {
        return;
    }
    switch (powerUp.type) {
        case ATTACK:
            DrawSword(powerUp,x,y);
            break;
        case DEFENSE:
            DrawShield(powerUp,x,y);
            break;
        case MAX_HP:
            DrawHealth(powerUp,x,y);
            break;
        default:
            break;
    }
}

void DrawKey(DoorKey_t key, int x, int y) {
    if(key.pickedUp) {
        return;
    }
    key.position.y++;
    key.position.z += y * CHUNK_SIZE;
    key.position.x += x * CHUNK_SIZE;

    keyModel.transform = MatrixRotateXYZ((Vector3){ 0.0f, fmodf((float)GetTime() * 50, 360) * DEG2RAD, 50.0f * DEG2RAD });
    DrawModel(keyModel, key.position, 0.10f, GOLD);
}

void DrawPotion(potion_t potion, int chunkX, int chunkY) {
    if(potion.pickedUp) {
        return;
    }
    potion.position.y = potion.position.y + sin((float)GetTime() * 2) * 0.1f + 0.5f;
    potion.position.x += 0.5f + chunkX * CHUNK_SIZE;
    potion.position.z += 0.5f + chunkY * CHUNK_SIZE;
    potionModel.transform = MatrixRotateXYZ((Vector3){ 90 * DEG2RAD, 0, 0});
    DrawModel(potionModel, potion.position, 0.015f, (Color){255,255,255,100});
}


void DrawDoor(door_t door,int x,int y) {

    if(door.opened){
        return;
    }
    door.position.y = (door.position.y)+0.5;
    door.position.x += 0.5f + x * CHUNK_SIZE;
    door.position.z += 0.5f + y * CHUNK_SIZE;
    DrawCubeCustom(doorUpTexture,(Vector3){ door.position.x, door.position.y+1, door.position.z },1.0f,1.0f,1.0f,WHITE);
    DrawCubeCustom(doorDownTexture,door.position, 1.0f, 1.0f, 1.0f, WHITE);
}

void DrawHealth(powerUp_t health, int x, int y){
    health.position.y = health.position.y + sin((float)GetTime() * 2) * 0.1f + 0.5f;
    health.position.x += 0.5f + x * CHUNK_SIZE;
    health.position.z += 0.5f + y * CHUNK_SIZE;
    powerUpHealthModel.transform = MatrixRotateXYZ((Vector3){ 0, fmodf((float)GetTime() * 50,360) * DEG2RAD, 0});
    DrawModel(powerUpHealthModel, health.position, 0.5f, CLITERAL(Color){ 200, 100, 100, 150 });
}

void DrawMonster(monster_t monster, int x, int y){

    if(monster.isDead){
        return;
    }
    monster.position.y = (monster.position.y)+1.5;
    monster.position.x += 0.5f + x * CHUNK_SIZE;
    monster.position.z += 0.5f + y * CHUNK_SIZE;

    DrawModel(monsterModel, monster.position, 0.25f,WHITE);
   
    render3DText(
        TextFormat("Health : %d \nArmor : %d\nDamage : %d",(int)monster.statistics.health,(int)monster.statistics.armor,(int)monster.statistics.damage),
        (Vector3){monster.position.x,(monster.position.y)+2,monster.position.z},
        2.0
    );


    


}














