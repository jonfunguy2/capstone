#include "game.h"

#include <string>

#include "player.h"
#include "raylib.h"

namespace game {

Game::Game(unsigned int screen_width, unsigned int screen_height,
           const std::string& title, unsigned int target_fps) noexcept
    : window_{rl::Window(screen_width, screen_height, title,
                         FLAG_WINDOW_RESIZABLE)},
      screen_width_(screen_width),
      screen_height_(screen_height),
      scale_{static_cast<float>(screen_width) / DEFAULT_SCREEN_WIDTH} {
    window_.SetTargetFPS(target_fps);
}

void Game::Run() {

    // added code (camera)
    camera_.offset = { window_.GetWidth()/2.5f, window_.GetHeight()/2.0f };
    camera_.rotation = 0.0f;
    camera_.zoom = 1.0f;
    // end of added code (camera)

    std::string player_texture_path =
        (resource_path_ / "sprites" / "jenny-walking.png").generic_string();

    rl::Texture player_texture = rl::Texture(player_texture_path);
    Player player(rl::Vector2(static_cast<float>(screen_width_) / 5,
                              static_cast<float>(screen_height_) / 5),
                  std::move(player_texture), rl::Vector2{10, 10});

    player_ = std::make_unique<Player>(std::move(player));

    std::string test_background_texture_path =
        (resource_path_ / "sprites" / "test-background.png").generic_string();

    rl::Texture background_texture = rl::Texture(test_background_texture_path);

    // added code for testing
    player_->SetPositionX(window_.GetWidth() / 2 - 64);
    player_->SetPositionY(window_.GetHeight() / 2 - 64);
    // end of added code for testing

    while (!window_.ShouldClose()) {
        if (window_.IsResized()) {
            HandleResize();
            // player_->SetPosition(static_cast<float>(screen_width_) / 2 - 64,
            //                      static_cast<float>(screen_height_) / 2 -
            //                      64);
        }

        HandleKeyboardEvents();
        rl::Rectangle source_rec = {
            0.0F, 0.0F, static_cast<float>(background_texture.GetWidth()),
            static_cast<float>(background_texture.GetHeight())};

        float height = static_cast<float>(window_.GetWidth()) * (9.0F / 16.0F);
        float width = static_cast<float>(height * 16.0F / 9.0F);

        float ratio = static_cast<float>(window_.GetWidth()) /
                      static_cast<float>(window_.GetHeight());

        rl::Rectangle dest_rec = {0.0F, 0.0F,
                                  static_cast<float>(window_.GetWidth()),
                                  static_cast<float>(window_.GetHeight())};

        // this if statement has been changed for testing
        if (ratio > 16.0F / 9.0F) {
            float width =
                static_cast<float>(window_.GetHeight()) * (16.0F / 9.0F);

            dest_rec.x = (window_.GetWidth() - width) / 2;
            dest_rec.y = 0.0F;
            dest_rec.width = width;

            //player_->SetPositionX(dest_rec.x + dest_rec.width / 6); commented out for testing
            //player_->SetPositionY(dest_rec.y + dest_rec.height * 3 / 5); commented out for testing

        } else if (ratio <= 16.0F / 9.0F) {
            float height =
                static_cast<float>(window_.GetWidth()) * (9.0F / 16.0F);

            dest_rec.x = 0.0F;
            dest_rec.y = (window_.GetHeight() - height) / 2;
            dest_rec.height = height;

            //player_->SetPositionX(dest_rec.x + dest_rec.width / 6); commented out for testing
            //player_->SetPositionY(dest_rec.y + dest_rec.height * 7 / 10); commented out for testing
        }
        else{
            float height =
                static_cast<float>(window_.GetWidth()) * (9.0F / 16.0F);

            dest_rec.x = 0.0F;
            dest_rec.y = (window_.GetHeight() - height) / 2;
            dest_rec.height = height;

        }

        // added code (camera)
        if(camera_mode_ == CameraMode::kDynamic && !moving_){
            UpdateCamera(CameraMode::kDynamic, CameraAxis::kBoth);
        }
        else if(camera_mode_ == CameraMode::kFixed && !moving_){
            UpdateCamera(CameraMode::kFixed, CameraAxis::kBoth);
        }
        // end of added code (camera)

        window_.BeginDrawing();
        BeginMode2D(camera_);
        window_.ClearBackground(SPACE);

        background_texture.Draw(source_rec, dest_rec);

        player_->Draw(scale_);

        dest_rec.DrawLines(rl::Color::RayWhite(), 2.0F);
        EndMode2D();
        window_.EndDrawing();
    }
}

void Game::HandleResize() {
    screen_width_ = window_.GetWidth();
    screen_height_ = window_.GetHeight();
    scale_ = static_cast<float>(screen_width_) / DEFAULT_SCREEN_WIDTH;
}

rl::Texture LoadTexture(const fs::path& texture_path) {
    return rl::Texture(texture_path.generic_string());
}

// Added camera update function
void Game::UpdateCamera(CameraMode camera_type, CameraAxis camera_axis){
    camera_.zoom += ((float)GetMouseWheelMove()*0.05f);
    if (camera_.zoom > 1.25f) camera_.zoom = 1.25f;
    else if (camera_.zoom < 0.75f) camera_.zoom = 0.75f;
    if(camera_type == CameraMode::kDynamic){
        static float minSpeed = 100;
        static float minEffectLength = 10;
        static float fractionSpeed = 1.2f;
        Vector2 diff = Vector2Subtract(player_->GetPosition(), camera_.target);
        float length = Vector2Length(diff);

        if (length > minEffectLength)
        {
            float speed = fmaxf(fractionSpeed*length, minSpeed);
            camera_.target = Vector2Add(camera_.target, Vector2Scale(diff, speed*GetFrameTime()/length));
        }

        if(camera_axis == CameraAxis::kLeftX){
            if(camera_.offset.x >= window_.GetWidth()){
            camera_.offset.x = window_.GetWidth();
            }
            else {
                camera_.offset.x += 10.0f;
            }
        }
        else if(camera_axis == CameraAxis::kRightX){
            if(camera_.offset.x <= 0.0f){
            camera_.offset.x = 0.0f;
            }
            else {
                camera_.offset.x -= 10.0f;
            }
        }
        else if(camera_axis == CameraAxis::kUpY){
            if(camera_.offset.y >= window_.GetHeight()*1.25f){
            camera_.offset.y = window_.GetHeight()*1.25f;
            }
            else {
                camera_.offset.y += 10.0f;
            }
        }
        else if(camera_axis == CameraAxis::kDownY){
            if(camera_.offset.y <= -window_.GetHeight()/2.0f){
            camera_.offset.y = -window_.GetHeight()/2.0f;
            }
            else {
                camera_.offset.y -= 10.0f;
            }
        }
        else{
            if(camera_.offset.x > window_.GetWidth()/1.96f){
            camera_.offset.x -= 10.0f;
            }
            else if (camera_.offset.x < window_.GetWidth()/2.0f){
                camera_.offset.x += 10.0f;
            }
            if(camera_.offset.y > window_.GetHeight()/1.96f){
            camera_.offset.y -= 10.0f;
            }
            else if (camera_.offset.y < window_.GetHeight()/2.0f){
                camera_.offset.y += 10.0f;
            }
        }
    }
    else{
        /*camera_.target = player_->GetPosition();
        camera_.offset = { window_.GetWidth()/2.0f, window_.GetHeight()/2.0f };
        float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;
        
        minX = fminf(source_rec.x, minX);
        maxX = fmaxf(source_rec.x + width, maxX);
        minY = fminf(source_rec.y, minY);
        maxY = fmaxf(source_rec.y + height, maxY);
        
        Vector2 max = GetWorldToScreen2D({ maxX, maxY }, camera_);
        Vector2 min = GetWorldToScreen2D({ minX, minY }, camera_);
        if (max.x < width) camera_.offset.x = width - (max.x - width/2.0);
        if (max.y < height) camera_.offset.y = height - (max.y - height/2.0);
        if (min.x > 0) camera_.offset.x = width/2.0 - min.x;
        if (min.y > 0) camera_.offset.y = height/2.0 - min.y;*/
    }
}
// End of added camera update function

/**
 * @brief Handle keyboard events, for example arrow keys, space bar, etc.
 */
// code for this function has been changed for testing
    void Game::HandleKeyboardEvents() {
        std::cout << "camera's x offset = " << camera_.offset.x << " and camera's y offset =  " << camera_.offset.y << std::endl;
        int key = GetKeyPressed();
        switch (key) {
            // Camera
            case KEY_C:
                if(camera_mode_ == CameraMode::kDynamic){
                    camera_mode_ = CameraMode::kFixed;
                }
                else{
                    camera_mode_ = CameraMode::kDynamic;
                }
                break;
            // Inventory
            case KEY_E:
                break;
            // Pause
            case KEY_P:
                break;
            default:
                break;
        };
        if(IsKeyDown(KEY_A)){player_->Move(Player::Direction::kLeft); UpdateCamera(CameraMode::kDynamic, CameraAxis::kLeftX); moving_ = true;}
        if(IsKeyDown(KEY_D)){player_->Move(Player::Direction::kRight); UpdateCamera(CameraMode::kDynamic, CameraAxis::kRightX); moving_ = true;}
        if(IsKeyDown(KEY_W)){player_->Move(Player::Direction::kUp); UpdateCamera(CameraMode::kDynamic, CameraAxis::kUpY); moving_ = true;}
        if(IsKeyDown(KEY_S)){player_->Move(Player::Direction::kDown); UpdateCamera(CameraMode::kDynamic, CameraAxis::kDownY); moving_ = true;}
        if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_W) && !IsKeyDown(KEY_S)){player_->Move(Player::Direction::kNone); moving_ = false;}
    }

}  // namespace game
