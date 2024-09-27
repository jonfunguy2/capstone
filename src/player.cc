#include "player.h"
#include <cmath>
namespace game {

Player::Player(const rl::Vector2& position, rl::Texture&& texture,
               const rl::Vector2& velocity)
    : Textured(std::move(texture)), GameObject(position) {}

// code for this function has been changed for testing
void Player::Move(Direction direction) {
    float deceleration = 0.95f;
    switch (direction) {
        case Direction::kUp:
            if(velocity_.y <= -8){
                velocity_.y = -8;
            }
            else{
                velocity_.y -= 2;
            }
            position_.y += velocity_.y;
            break;
        case Direction::kDown:
            if(velocity_.y >= +8){
                velocity_.y = +8;
            }
            else{
                velocity_.y += 2;
            }
            position_.y += velocity_.y;
            break;
        case Direction::kLeft:
            if(velocity_.x <= -8){
                velocity_.x = -8;
            }
            else{
                velocity_.x -= 2;
            }
            position_.x += velocity_.x;
            break;
        case Direction::kRight:
            if(velocity_.x >= 8){
                velocity_.x = 8;
            }
            else{
                velocity_.x += 2;
            }
            position_.x += velocity_.x;
            break;
        // added code (movement)
        case Direction::kNone:
            if(abs(velocity_.x) >= deceleration || abs(velocity_.y) >= deceleration){
                velocity_.x *= deceleration;
                velocity_.y *= deceleration;
                position_.x += velocity_.x;
                position_.y += velocity_.y;
            }
            else{
                velocity_.x = 0;
                velocity_.y = 0;
            }
            break;
        default:
            break;
        // end of added code (movement)
    }
}
// end of function that was changed

// added function get velocity
Vector2 Player::GetVelocity(){
    return Vector2{velocity_.x, velocity_.y};
}
// end of added function get velocity

auto operator<<(std::ostream& os, const Player& player) -> std::ostream& {
    os << "Player position: " << player.position_.x << ", "
       << player.position_.y << std::endl;
    return os;
}

void Player::Draw() { Draw(0.0F); }

void Player::Draw(float scale) {
    static rl::Rectangle frame_rec = {
        position_.x, position_.y, static_cast<float>(texture_.GetWidth()) / 6,
        static_cast<float>(texture_.GetHeight())};

    frame_counter_ += 1;
    if (frame_counter_ >= 12) {
        frame_counter_ = 0;
        frame_rec.x += static_cast<float>(texture_.GetWidth()) / 6;

        if (frame_rec.x >=
            static_cast<float>(texture_.GetWidth()) + position_.x) {
            frame_rec.x = position_.x;
        }
    }

    rl::Rectangle rec = {position_.x, position_.y,
                         static_cast<float>(texture_.GetWidth()),
                         static_cast<float>(texture_.GetHeight())};

    float x = (static_cast<int>(frame_rec.x) + 127) & ~127;
    float y = (static_cast<int>(frame_rec.y) + 127) & ~127;

    rl::Rectangle source_rec = {x, y,
                                static_cast<float>(texture_.GetWidth()) / 6,
                                static_cast<float>(texture_.GetHeight())};

    float width = scale * static_cast<float>(texture_.GetWidth()) / 6;
    float height = scale * static_cast<float>(texture_.GetHeight());

    rl::Rectangle dest_rec = {position_.x, position_.y, width, height};

    texture_.Draw(source_rec, dest_rec, {0, 0});
}

void Player::Draw(float scale, rl::Vector2 position) {
    position_ = position;
    Draw(scale);
}

void Player::Update() {}



}  // namespace game
