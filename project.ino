#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int pin_up = 7;
int pin_down = 8;

bool finish = false;

float FPS = 2;
float frameDelay = 1000 / FPS;
float frameStart, frameTime;

const int num_row = 2;
const int num_column = 16;

String player_sym = ">";
String enemy_sym = "0";

const int max_enemy = 8;
int max_spawn_delay = 10;
int min_spawn_delay = 3;
int spawn_decay = 20;
int spawn_decay_count = 0;
int spawn_delay = max_spawn_delay;
int spawn_count = spawn_delay;
int enemy_list_row[max_enemy];
int enemy_list_column[max_enemy];
int num_enemy = 0;

const int player_column = 3;
int player_row = 0;

bool up_botton_state = false;
bool down_botton_state = false;

void setup()
{
    lcd.begin(num_column, num_row);
    pinMode(pin_up, INPUT);
    pinMode(pin_down, INPUT);
    Serial.begin(9600);
}

void loop()
{
    if (!finish)
    {
        frameStart = millis();

        spawn_count++;

        if (spawn_count >= spawn_delay && num_enemy < max_enemy)
        {
            for (int i = 0; i < max_enemy; i++)
            {
                if (enemy_list_row[i] == NULL && enemy_list_column[i] == NULL)
                {
                    randomSeed(millis() - frameStart);
                    int rand = random(1, num_row + 1);
                    enemy_list_row[i] = rand;
                    enemy_list_column[i] = num_column - 1;
                    break;
                }
            }
            spawn_count = 0;
            num_enemy++;
            spawn_decay_count++;
        }

        int event = get_event();
        switch (event)
        {
        case 0:
            break;
        case 1:
            if (player_row != 0)
            {
                player_row = 0;
            }
            break;
        case 2:
            if (player_row != 1)
            {
                player_row = 1;
            }
            break;
        default:
            break;
        }

        render(player_sym, player_column, player_row);

        for (int i = 0; i < max_enemy; i++)
        {
            if (enemy_list_row[i] != NULL && enemy_list_column[i] != NULL)
            {
                render(enemy_sym, enemy_list_column[i], enemy_list_row[i]-1);
                if (enemy_list_row[i] - 1 == player_row && enemy_list_column[i] == player_column)
                {
                    finish = true;
                }
                enemy_list_column[i]--;
                if (enemy_list_column[i] <= 0)
                {
                    enemy_list_column[i] = NULL;
                    enemy_list_row[i] = NULL;
                    num_enemy--;
                }
            }
        }

        if (spawn_decay_count >= spawn_decay)
        {
            spawn_decay_count = 0;
            if (spawn_decay > min_spawn_delay)
            {
                spawn_delay--;
            }
        }

        frameTime = millis() - frameStart;
        if (frameTime < frameDelay)
        {
            delay(frameDelay - frameTime);
        }

        lcd.clear();
    }
    else
    {
        bool click = gameover();
        if (click)
        {
            lcd.clear();
            reset();
            finish = false;
        }
    }
    
}

void render(String symbol, int column, int row)
{
    lcd.setCursor(column, row);
    lcd.print(symbol);
}

// none = 0, up = 1, down = 2
int get_event()
{
    int up_botton = digitalRead(pin_up);
    int down_botton = digitalRead(pin_down);

    if (up_botton == 1 && down_botton == 1 && !up_botton_state && !down_botton_state)
    {
        up_botton_state = true;
        down_botton_state = true;

        return 0;
    }
    else if (up_botton == 1 && down_botton == 0 && !up_botton_state)
    {
        up_botton_state =  true;
        down_botton_state = false;

        return 1;
    }
    else if (up_botton == 0 && down_botton == 1 && !down_botton_state)
    {
        up_botton_state =  false;
        down_botton_state = true;

        return 2;
    }
    else if (up_botton == 0 && down_botton == 0)
    {
        up_botton_state =  false;
        down_botton_state = false;

        return 0;
    }
    else
    {
        return 0;
    }
}

bool gameover()
{
    int up_botton = digitalRead(pin_up);
    int down_botton = digitalRead(pin_down);

    render("Game Over!", 3, 0);
    render("Push any botton", 0, 1);

    if (up_botton == 1 || down_botton == 1)
    {
        return true;
    }
    return false;
}

void reset()
{
    spawn_decay_count = 0;
    spawn_delay = max_spawn_delay;
    spawn_count = spawn_delay;
    num_enemy = 0;

    for (int i = 0; i < max_enemy; i++)
    {
        enemy_list_column[i] = NULL;
        enemy_list_row[i] = NULL;
    }

    player_row = 0;

    up_botton_state = false;
    down_botton_state = false;
}