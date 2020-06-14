set(COMPONENT_SRCDIRS ./src 
                      ./src/v2 
                      ./src/v2/lcd 
                      ./src/canvas 
                      ./src/canvas/fonts 
                      ./src/v2/nano_engine 
)

set(COMPONENT_ADD_INCLUDEDIRS "./src")

set(COMPONENT_REQUIRES ESP32tools)

register_component()
