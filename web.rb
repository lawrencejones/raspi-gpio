# encoding: UTF-8
#!/bin/ruby
# Simple little web API to expose the gpio C library.
# Should serve live state information about all the pins.

require 'rubygems'
require 'ffi'
require 'em-websocket'
require 'coffee-script'
require 'haml'
require 'thin'
require 'sinatra/base'

system "make"

class PinStruct < FFI::Struct
  layout :chipIndex, :int, 
    :memIndex, :int,
    :state, :int,
    :value, :int
end

module Gpio extend FFI::Library
  ffi_lib File.join(File.expand_path('bin'), 'gpio')
  attach_function :initialiseGpios, [], :pointer
  attach_function :chipIndexToMem, [:int], :int
  attach_function :mallocPin, [:int], :pointer
  attach_function :pinStatus, [:int], :pointer
  attach_function :setPin, [:int, :int], :void
end

gpio = Gpio.initialiseGpios

EM.run do

  # Define the app behaviour
  class App < Sinatra::Base
    # Define the get to root
    get '/' do
      haml :index    # render index view
    end

    # Defines script asset serving
    get '/script.js' do
      coffee(:script)
    end

    # Testing
    get '/status' do
      puts 'hello'
      puts Gpio.pinStatus 5
    end

  end

  EM::WebSocket.start(:host => '0.0.0.0', :port => 4567) do |ws|
    ws.onopen {
      puts 'this'
      ws.send "connected!!!!"
    }
    ws.onmessage { |message|
      puts 'message received'
    }
  end

  Thin::Server.start App, '0.0.0.0', 3000

end
