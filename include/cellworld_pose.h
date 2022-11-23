#pragma once
#include <json_cpp/json_object.h>
#include <json_cpp/json_vector.h>
#include <cell_world/location.h>

namespace cell_world {
    struct Pose_part: json_cpp::Json_object {
        Json_object_members(
                Add_member(part);
                Add_member(location);
                Add_member(camera);
                Add_member(score);
        )
        std::string part{};
        Location location{};
        int camera{};
        float score{};
        bool operator == (const Pose_part &o) const {
            return camera == o.camera && location==o.location && part==o.part && score == o.score;
        }
    };
}