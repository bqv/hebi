module Plugin where

import Message

import qualified Data.Text as T
import System.Directory
import Data.List
import System.Posix.IO
import System.Posix.Types

import Foreign.C.Types
import Foreign.C.String
import Foreign.Marshal.Alloc

handle :: Message -> IO ()
handle (Message _ Invite params) = let
                                    (Short name (Long chans)) = params
                                   in
                                    send ("JOIN "++(T.unpack chans))
handle msg = getDirectoryContents "scripts" >>=
             return . filter (isSuffixOf ".hs") >>=
             mapM_ (runScript msg) . map ("./script/"++)

runScript :: Message -> FilePath -> IO ()
runScript msg script = log_debug ("Running Script "++script) >>
                       log_debug ("HS-"++(show msg))

foreign import ccall send_hs :: CString -> IO ()
foreign import ccall log_debug_hs :: CString -> IO ()
foreign import ccall log_info_hs :: CString -> IO ()
foreign import ccall log_warn_hs :: CString -> IO ()
foreign import ccall log_error_hs :: CString -> IO ()
foreign import ccall log_fatal_hs :: CString -> IO ()

send :: String -> IO ()
send s = newCString s >>= \cs ->
            send_hs cs >>
            free cs

log_debug :: String -> IO ()
log_debug s = newCString s >>= \cs ->
                log_debug_hs cs >>
                free cs

log_info :: String -> IO ()
log_info s = newCString s >>= \cs ->
                log_info_hs cs >>
                free cs

log_warn :: String -> IO ()
log_warn s = newCString s >>= \cs ->
                log_warn_hs cs >>
                free cs

log_error :: String -> IO ()
log_error s = newCString s >>= \cs ->
                log_error_hs cs >>
                free cs

log_fatal :: String -> IO ()
log_fatal s = newCString s >>= \cs ->
                log_fatal_hs cs >>
                free cs

handle_hs :: CString -> IO ()
handle_hs cs = log_debug "In Haskell" >>
               peekCString cs >>=
               handle . read

run_hs :: Fd -> IO ()
run_hs fd = forever (fdRead fd 4096 >>= \(str, _) -> handle (read $ init str))
        where
            forever x = x >> forever x

foreign export ccall run_hs :: Fd -> IO ()
foreign export ccall handle_hs :: CString -> IO ()
