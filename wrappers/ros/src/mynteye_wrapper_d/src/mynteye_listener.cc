// Copyright 2018 Slightech Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/image_encodings.hpp>
#include <sensor_msgs/msg/image.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
// #include <mynteye_wrapper_d/GetParams.h>
#define CONFIGURU_IMPLEMENTATION 1
#include "configuru.hpp"
using namespace configuru; // NOLINT

namespace enc = sensor_msgs::image_encodings;

class MYNTEYEListener {
public:
  MYNTEYEListener(rclcpp::Node::SharedPtr node) : node_(node), color_count_(0) {
    //    color_sub_ =
    //    node_->create_subscription<sensor_msgs::msg::Image>("mynteye/left/image_color",
    //         1, std::bind(&MYNTEYEListener::colorCallback, this,
    //         std::placeholders::_1));
    image_transport::ImageTransport it(node_);
    color_sub_ =
        it.subscribe("mynteye/left/image_color", // Replace with your topic
                     10,
                     std::bind(&MYNTEYEListener::colorCallback, this,
                               std::placeholders::_1));
    depth_sub_ =
        it.subscribe("mynteye/depth/image_raw", // Replace with your topic
                     10,
                     std::bind(&MYNTEYEListener::depthCallback, this,
                               std::placeholders::_1));

    //    std::string img_intri = getCameraCalibInfo(0u);
    //    std::string img_extri = getCameraCalibInfo(1u);
    //    std::string imu_intri = getCameraCalibInfo(2u);
    //    std::string imu_extri = getCameraCalibInfo(3u);
    //
    //    auto img_intri_info = parse_string(img_intri.c_str(), JSON, "log");
    //    auto img_extri_info = parse_string(img_extri.c_str(), JSON, "log");
    //    auto imu_intri_info = parse_string(imu_intri.c_str(), JSON, "log");
    //    auto imu_extri_info = parse_string(imu_extri.c_str(), JSON, "log");
    //    std::cout << "IMG_INTRINSICS:" << img_intri_info << std::endl
    //              << "IMG_EXTRINSICS_RTOL:" << img_extri_info << std::endl
    //              << "IMU_INTRINSICS:" << imu_intri_info << std::endl
    //              << "IMU_EXTRINSICS:" << imu_extri_info << std::endl;

    cv::namedWindow("color", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("depth", cv::WINDOW_AUTOSIZE);
  }

  ~MYNTEYEListener() { cv::destroyAllWindows(); }

  //  std::string getCameraCalibInfo(unsigned int type_) {
  //    ros::ServiceClient client =
  //        nh_.serviceClient<mynteye_wrapper_d::GetParams>("/mynteye_wrapper_d_node/get_params");
  //    mynteye_wrapper_d::GetParams srv;
  //    srv.request.key = type_;
  //    if (client.call(srv)) {
  //      return srv.response.value;
  //    } else {
  //      ROS_ERROR("Failed to call service GetParams, make sure you have launch
  //      mynteye device SDK nodelet"); return "null";
  //    }
  //  }
  //
  void colorCallback(const sensor_msgs::msg::Image::ConstSharedPtr &msg) {
    cv_bridge::CvImageConstPtr cv_ptr;
    try {
      cv_ptr = cv_bridge::toCvShare(msg, enc::RGB8);
    } catch (cv_bridge::Exception &e) {
      // ROS_ERROR_STREAM("cv_bridge exception: " << e.what());
      return;
    }
    ++color_count_;
    // ROS_INFO_STREAM("color: " << color_count_);

    cv::imshow("color", cv_ptr->image);
    cv::waitKey(3);
  }

  void depthCallback(const sensor_msgs::msg::Image::ConstSharedPtr &msg) {
    cv_bridge::CvImageConstPtr cv_ptr;
    try {
      if (enc::isColor(msg->encoding)) {
        cv_ptr = cv_bridge::toCvShare(msg, enc::RGB8);
      } else if (msg->encoding == enc::MONO16) {
        cv_ptr = cv_bridge::toCvShare(msg, enc::MONO16);
      } else {
        cv_ptr = cv_bridge::toCvShare(msg, enc::MONO8);
      }
    } catch (cv_bridge::Exception &e) {
      // ROS_ERROR_STREAM("cv_bridge exception: " << e.what());
      return;
    }
    cv::imshow("depth", cv_ptr->image);
    cv::waitKey(3);
  }

  std::uint64_t colorCount() const { return color_count_; }

private:
  rclcpp::Node::SharedPtr node_;
  image_transport::Subscriber color_sub_;
  image_transport::Subscriber depth_sub_;

  std::uint64_t color_count_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("mynteye_listener_d");

  MYNTEYEListener l(node);

  rclcpp::spin(node);

  // ROS_INFO_STREAM("time beg: " << std::fixed << time_beg << " s");
  // ROS_INFO_STREAM("time end: " << std::fixed << time_end << " s");
  // ROS_INFO_STREAM("time cost: " << elapsed << " s");
  // ROS_INFO_STREAM("color count: " << l.colorCount() << ", "
  //     << (l.colorCount() / elapsed) << " fps");

  return 0;
}
