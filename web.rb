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

class Chip < FFI::Struct
  layout :pins, :pointer

  def pin(n)
    PinStruct.new self[:pins].get_array_of_pointer(0,26)[n-1]
  end
end

module Gpio extend FFI::Library
  ffi_lib File.join(File.expand_path('bin'), 'gpio')
  attach_function :initialiseGpioAccess, [], :pointer
  attach_function :initialiseChip, [], :pointer
  attach_function :chipIndexToMem, [:int], :int
  attach_function :mallocPin, [:int], :pointer
  attach_function :pinStatus, [:int], :pointer
  attach_function :setPin, [:int, :int], :void
end

EM.run do

  Gpio.initialiseGpioAccess()
  chip = Chip.new(Gpio.initialiseChip())

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
    get '/status/:pin' do
      pin_no = params[:pin].to_i
			puts chip.pin(pin_n)[:value]
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
