module Plugin where

import Message

import qualified Data.Text as T

import Foreign.C.Types
import Foreign.C.String
import Foreign.Marshal.Alloc

foreign import ccall send_hs :: CString -> IO ()
foreign import ccall log_debug_hs :: CString -> IO ()

send :: String -> IO ()
send s = newCString s >>= \cs ->
            send_hs cs >>
            free cs

log_debug :: String -> IO ()
log_debug s = newCString s >>= \cs ->
                send_hs cs >>
                free cs

handle :: Message -> IO ()
handle (Message _ Invite params) = let
                                    (Short name (Long chans)) = params
                                   in
                                    log_debug "In Haskell" >>
                                    send ("JOIN "++(T.unpack chans))
handle msg = log_debug "In Haskell" >>
             log_debug ("HS:"++(show msg)) >>
             send ("PRIVMSG ##doge :haskell")

handle_hs :: CString -> IO ()
handle_hs cs = peekCString cs >>= handle . read

foreign export ccall handle_hs :: CString -> IO ()
