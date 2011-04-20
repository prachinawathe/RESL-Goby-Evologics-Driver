// copyright 2011 t. schneider tes@mit.edu
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this software.  If not, see <http://www.gnu.org/licenses/>.

#include "moos_node.h"

#include "goby/util/logger.h"

#include "goby/acomms/acomms_helpers.h"

using goby::util::glogger;

goby::moos::MOOSNode::MOOSNode()
{
    ZeroMQNode::connect_inbox_slot(&goby::moos::MOOSNode::inbox, this);
        
    glogger().add_group("in_hex", util::Colors::green, "Goby MOOS (hex) - Incoming");
    glogger().add_group("out_hex", util::Colors::magenta, "Goby MOOS (hex) - Outgoing");

}


void goby::moos::MOOSNode::inbox(core::MarshallingScheme marshalling_scheme,
                                 const std::string& identifier,
                                 const void* data,
                                 int size)
{
    if(marshalling_scheme == goby::core::MARSHALLING_MOOS)
    {
        CMOOSMsg msg;
        std::string bytes(static_cast<const char*>(data), size);
        glogger() << group("in_hex") << goby::acomms::hex_encode(bytes) << std::endl;
        MOOSSerializer::parse(&msg, bytes);
        moos_inbox(msg);
    }
    
}

void goby::moos::MOOSNode::publish(CMOOSMsg& msg)
{            

    std::string bytes;
    MOOSSerializer::serialize(msg, &bytes);
    
    glogger() << group("out_hex") << goby::acomms::hex_encode(bytes) << std::endl;

    ZeroMQNode::publish(goby::core::MARSHALLING_MOOS, "CMOOSMsg/" + msg.GetKey() + "/", &bytes[0], bytes.size());
}

void goby::moos::MOOSNode::subscribe(const std::string& full_or_partial_moos_name)
{
    ZeroMQNode::subscribe(goby::core::MARSHALLING_MOOS, full_or_partial_moos_name);
}

