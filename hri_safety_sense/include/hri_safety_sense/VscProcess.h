/*
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __VSC_PROCESS_INCLUDED__
#define __VSC_PROCESS_INCLUDED__

/**
 * ROS Includes
 */
#include <ros/ros.h>

#include <any_node/any_node.hpp>

#include "hri_safety_sense/EmergencyStop.h"
#include "hri_safety_sense/KeyValue.h"
#include "hri_safety_sense/KeyString.h"

/**
 * HRI_COMMON Includes
 */
#include "MsgHandler.h"
#include "hri_safety_sense/VehicleMessages.h"
#include "hri_safety_sense/VehicleInterface.h"
#include "hri_safety_sense/MessageConfigure.h"
#include "hri_safety_sense/KeyValueResp.h"
#include "hri_safety_sense/RemoteStatus.h"

namespace hri_safety_sense {

// Diagnostics
struct ErrorCounterType {
  uint32_t sendErrorCount;
  uint32_t invalidRxMsgCount;
};

/**
 * Local Definitions
 */
const unsigned int VSC_INTERFACE_RATE = 50; /* 50 Hz */
const unsigned int VSC_HEARTBEAT_RATE = 20; /* 20 Hz */

class VscProcess:public any_node::Node {
 public:
  VscProcess() = delete; // constructor needs to take a shared_ptr to a ros::Nodehandle instance.
  VscProcess(any_node::Node::NodeHandlePtr nh);
  ~VscProcess() override = default;
  virtual bool update(const any_worker::WorkerEvent& event);

  // initialize subscribers and everything here
  bool init() override;
  // cleanup
  void cleanup() override;

  // Main loop
  void processOneLoop(const ros::TimerEvent&);

  // ROS Callback's
  bool EmergencyStop(EmergencyStop::Request &req, EmergencyStop::Response &res);
  bool KeyValue(KeyValue::Request &req, KeyValue::Response &res);
  bool KeyString(KeyString::Request &req, KeyString::Response &res);
  bool GetKeyValue(KeyValue::Request &req, KeyValue::Response &res);
  bool ConfigureMessages(MessageConfigure::Request &req,MessageConfigure::Response & res);
 private:

  void readFromVehicle();
  int handleHeartbeatMsg(VscMsgType& recvMsg);
  int handleFeedbackMsg(VscMsgType& recvMsg);
  int handleRemoteUpdate(VscMsgType& recvMsg);

  // Local State
  uint32_t myEStopState = 0;
  ErrorCounterType errorCounts;

  // ROS
//  ros::Timer mainLoopTimer;
  ros::ServiceServer estopServ, keyValueServ, keyStringServ, keyRequestServ, configureMessageServ;
  ros::Publisher estopPub;
  ros::Publisher keyValuesPub;
  ros::Publisher remoteStatusPub;
  ros::Time lastDataRx, lastTxTime;

  // Message Handlers
  MsgHandler* joystickHandler = nullptr;

  /* File descriptor for VSC Interface */
  VscInterfaceType* vscInterface = nullptr;

};

} // namespace


#endif
