# encoding: UTF-8
#!/bin/ruby
# Simple little web API to expose the gpio C library.
# Should serve live state information about all the pins.

require 'rubygems'
require 'em-websocket'
require 'coffee-script'
require 'haml'
require 'thin'
require 'sinatra/base'

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
