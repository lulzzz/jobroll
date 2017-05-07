require 'net/http'

ENV["RAILS_ENV"] ||= "development"
require File.join(File.dirname(__FILE__), *%w[.. .. config environment])

Rfid.connection
Rfid.create(uid: 123)

#uri = URI.parse('http://localhost:3000/rfids.json')

#some_data = { :uid => '6' }
#resp = Net::HTTP.post_form(uri.request_uri, some_data)
#p resp

#http = Net::HTTP.new(uri.host, uri.port)


#request = Net::HTTP::Post.new(uri.request_uri)
#request.set_form_data({:uid => 321})

#resp = http.request(request)
#p resp
