#include "renderer.h"
#define DEBUG_INFO_LINE_COUNT 5
drawBundle_t drawBundle = {0, 0, 0, {0, 0, 0}, {0, 0, 0}, 0, 0};

void initRenderer(player_t * player) {
    drawBundle.player = player;
}

void DrawMap(chunkedMap_t map) {

    for(int i = 0; i < map.width; i++) {
        for(int j = 0; j < map.height; j++) {
            if(map.chunks[i][j].x != -1) {
                //DrawChunk(map.chunks[i][j]);
                DrawCube((Vector3) {0,0,0}, 1.0f, 1.0f, 1.0f, CLITERAL(Color) {125, 125, 125, 125});
            }
        }
    }
}

void DrawChunk(chunk_t chunk) {
    for(int i = chunk.x * CHUNK_SIZE; i < (chunk.x + 1) * CHUNK_SIZE; i++) {
        for(int j = chunk.y * CHUNK_SIZE; j < (chunk.y + 1) * CHUNK_SIZE; j++) {
            switch (chunk.chunk[i % CHUNK_SIZE][j % CHUNK_SIZE]) {
                case 0:
                    DrawCube((Vector3) {i + 0.5, -0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, CLITERAL(Color) {255, 255, 255, 255});
                    DrawCubeWires((Vector3) {i + 0.5, -0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON);
                    break;
                case 1:
                    for (int k = 0; k < WALL_HEIGHT; k++) {
                        DrawCube((Vector3) {i + 0.5, k + 0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, CLITERAL(Color) {255, 255, 255, 255});
                        DrawCubeWires((Vector3) {i + 0.5, k + 0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON);
                    }
                    break;

                case 2:
                    DrawCube((Vector3) {i + 0.5, 0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, CLITERAL(Color) {255, 100, 100, 100});
                    DrawCubeWires((Vector3) {i + 0.5, 0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON);
                    break;
                default:
                    DrawCube((Vector3) {i + 0.5, -0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, CLITERAL(Color) {0, 255, 100, 255});
                    DrawCubeWires((Vector3) {i + 0.5, -0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON);
                    break;

            }
            if(drawBundle.drawCeiling) {
                DrawCube((Vector3){ i + 0.5, WALL_HEIGHT + 1 - 0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, CLITERAL(Color){ 255, 255, 255, 255} );
                DrawCubeWires((Vector3){ i + 0.5, WALL_HEIGHT +1 -0.5, j + 0.5}, 1.0f, 1.0f, 1.0f, MAROON );
            }
        }

    }
}

void DrawOverlay() {
    if(drawBundle.drawOverlay) {
        DrawRectangle(600, 50, 200, 60, CLITERAL(Color){ 50, 255, 50, 100});
        DrawText(TextFormat("- Health: %03.2f / %03.2f", drawBundle.player->statistics.health,  drawBundle.player->statistics.health), 610, 60, 15, BLACK);
        DrawText(TextFormat("- Armor: %03.2f, Attack: %03.2f", drawBundle.player->statistics.armor, drawBundle.player->statistics.damage), 610, 75, 15, BLACK);
        DrawText(TextFormat("- Keys: %d, Power-up %d", drawBundle.player->inventory.keyCount, drawBundle.player->inventory.potionCount), 610, 90, 15, BLACK);
    }
}

void DrawDebug() {

    const char * infos[DEBUG_INFO_LINE_COUNT] = {
            TextFormat("x: %f, y: %f, z: %f", drawBundle.player->camera->position.x, drawBundle.player->camera->position.y, drawBundle.player->camera->position.z),
            TextFormat("cx: %d, cy: %d", ((int)drawBundle.player->camera->position.x) / CHUNK_SIZE, ((int)drawBundle.player->camera->position.z) / CHUNK_SIZE),
            TextFormat("looking at: (%f, %f, %f)", drawBundle.player->camera->target.x, drawBundle.player->camera->target.y, drawBundle.player->camera->target.z),
            TextFormat("move: (%f, %f, %f)", drawBundle.movement.x, drawBundle.movement.y, drawBundle.movement.z),
            TextFormat("rot: (%f, %f, %f)", drawBundle.direction.x, drawBundle.direction.y, drawBundle.direction.z)
    };

    if(drawBundle.drawDebug) {
        for(int i = 0; i < DEBUG_INFO_LINE_COUNT; i++) {
            DrawText(infos[i], 10, 25 + 15 * i, 10, WHITE);
        }
    }
}
void Render(chunkedMap_t map) {
        ClearBackground(BLACK);

        BeginMode3D(*drawBundle.player->camera);

        DrawMap(map);
        render3DText("caaca", (Vector3){0, 0, -3}, (Vector3){ 10, 3, 10});


        EndMode3D();

        DrawOverlay();
        DrawDebug();

}


drawBundle_t getDrawBundle() {
    return drawBundle;
}

void setDrawBundle(drawBundle_t bundle) {
    drawBundle = bundle;
}



#include "../../includes/rlgl.h"

#include <stddef.h>     // Required for: NULL
#include <math.h>       // Required for: sinf()

#define RAYLIB_NEW_RLGL

// Draw a codepoint in 3D space
static void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, Color tint);
// Draw a 2D text in 3D space
static void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, Color tint);
// Measure a text in 3D. For some reason `MeasureTextEx()` just doesn't seem to work so i had to use this instead.
static Vector3 MeasureText3D(Font font, const char *text, float fontSize, float fontSpacing, float lineSpacing);

int render3DText(char * text, Vector3 position, Vector3 orientation)
{
    Vector3 textDimension;
    // Initialization
    //--------------------------------------------------------------------------------------
    Font font = GetFontDefault();

    textDimension = MeasureText3D(GetFontDefault(), text, 8.0f, 1.0f, 0.0f);
    position.x -= textDimension.x/2.0f;

    rlPushMatrix();

    Vector3 posPlayer = drawBundle.player->camera->position;

    float angleBetweenVectors = atan2f(posPlayer.x - position.x, posPlayer.z - position.z) * 180.0f / PI;
    
    rlRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    rlRotatef(angleBetweenVectors, 0.0f, 0.0f, -1.0f);

    DrawText3D(GetFontDefault(), text, position, 8.0f, 5.0f, 0.0f, RED);
    rlPopMatrix();

    UnloadFont(font);
    return 0;
}

//--------------------------------------------------------------------------------------
// Module Functions Definitions
//--------------------------------------------------------------------------------------
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


















