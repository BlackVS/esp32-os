cmake_minimum_required(VERSION 3.5)

if(CONFIG_ESP32_BOARD_NNC2019)
 set(COMPONENT_ADD_INCLUDEDIRS . hardware console resources ./boards/nnc2019)
 set(COMPONENT_SRCDIRS . hardware console resources ./boards/nnc2019)
elseif(CONFIG_ESP32_BOARD_M5STICKC)
 set(COMPONENT_ADD_INCLUDEDIRS . hardware console resources ./boards/m5stickc)
 set(COMPONENT_SRCDIRS . hardware console resources ./boards/m5stickc)
elseif(CONFIG_ESP32_BOARD_TTGO)
 set(COMPONENT_ADD_INCLUDEDIRS . hardware console resources ./boards/ttgo)
 set(COMPONENT_SRCDIRS . hardware console resources ./boards/ttgo)
endif()

#set( COMPONENT_ADD_LDFLAGS+= -z muldefs)

#set(COMPONENT_PRIV_INCLUDEDIRS proto-c)
#set(COMPONENT_PRIV_REQUIRES protobuf-c)

register_component()
