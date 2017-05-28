module Plugin where

import Message

import qualified Data.Text as T

import Foreign.C.Types
import Foreign.C.String
import Foreign.Marshal.Alloc

foreign import ccall send_hs :: CString -> IO ()

send :: String -> IO ()
send s = newCString s >>= \cs ->
            send_hs cs >>
            free cs

handle :: Message -> IO ()
handle (Message _ Invite params) = let
                                    (Short name (Long chans)) = params
                                   in
                                    putStrLn "In Haskell" >>
                                    send ("JOIN "++(T.unpack chans))
handle _ = return ()

handle_hs :: CString -> IO ()
handle_hs cs = peekCString cs >>= handle . read

foreign export ccall handle_hs :: CString -> IO ()
