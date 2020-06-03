cmake_minimum_required(VERSION 3.5)

#idf_component_register(SRCS 
set(COMPONENT_SRCDIRS .
                    hardware
                    console
                    resources
                    )
set(COMPONENT_ADD_INCLUDEDIRS .
                    hardware
                    console
                    resources
                    )

set( COMPONENT_ADD_LDFLAGS+= -z muldefs)

#set(COMPONENT_PRIV_INCLUDEDIRS proto-c)
#set(COMPONENT_PRIV_REQUIRES protobuf-c)

register_component()
