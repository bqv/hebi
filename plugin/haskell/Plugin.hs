module Plugin where

import Message

import qualified Data.Text as T
import System.Directory
import Data.List
import System.Posix.IO
import System.Posix.Types
import Control.Monad.Reader
import Control.Concurrent
import Data.Time.Clock
import System.Exit
import System.IO.Error

import Foreign.Ptr
import Foreign.C.Types
import Foreign.C.String
import Foreign.C.Error
import Foreign.Marshal.Alloc

import Debug.Trace

foreign import ccall safe "read"
    c_safe_read :: CInt -> Ptr CChar -> CSize -> IO CSize

fdRead_ :: Fd -> ByteCount -> IO String
fdRead_ fd nbytes = allocaBytes (fromIntegral nbytes) doRead
        where
            doRead :: Ptr CChar -> IO String
            doRead buf = c_safe_read (fromIntegral fd) buf nbytes >>=
                         handleLength buf . fromIntegral
            handleLength :: Ptr CChar -> Int -> IO String
            handleLength _ (-1) = throwErrno "fdRead_"
            handleLength _   0  = ioError undefined
            handleLength buf n  = fmap traceShowId $ peekCStringLen (buf, fromIntegral n)


run_hs :: Fd -> Fd -> IO ()
run_hs datafd logfd = runReaderT (log_debug $ "Working with FDs "++(show datafd)++","++(show logfd)) logfd >>
                      newEmptyMVar >>= \pong ->
                         forkIO (pingLoop pong) >>
                         forever (fdRead_ datafd 4096 >>= \str ->
                           let
                             msg = read str
                           in
                             pongFilter pong msg $ runReaderT (handle msg) logfd)
        where
            forever x = x >> forever x

foreign export ccall run_hs :: Fd -> Fd -> IO ()

pingLoop :: MVar () -> IO ()
pingLoop pong = threadDelay 5000000 >>
                runReaderT (send "PING :haskell") 0 >>
                getCurrentTime >>=
                waitLoop
        where
            waitLoop :: UTCTime -> IO ()
            waitLoop time = tryTakeMVar pong >>=
                            doCheck time
            doCheck :: UTCTime -> Maybe () -> IO ()
            doCheck time (Just ()) = pingLoop pong
            doCheck time (Nothing) = getCurrentTime >>=
                                     return . flip diffUTCTime time >>=
                                     checkPong . floor >>
                                     pingLoop pong 
                            
            checkPong :: Int -> IO ()
            checkPong x | x <= 240  = return ()
                        | otherwise = exitFailure

pongFilter :: MVar () -> Message -> IO () -> IO ()
pongFilter pong msg cont = case command msg of
                             Pong -> tryPutMVar pong () >>
                                     return ()
                             _ -> cont

handle :: Message -> ReaderT Fd IO ()
handle (Message _ Invite params) = let
                                    (Short name (Long chans)) = params
                                   in
                                    send ("JOIN "++(T.unpack chans))
handle msg = log_debug "In Haskell" >>
             liftIO (getDirectoryContents "scripts") >>=
             return . filter (isSuffixOf ".hs") >>=
             mapM_ (runScript msg) . map ("./script/"++)

runScript :: Message -> FilePath -> ReaderT Fd IO ()
runScript msg script = log_debug ("Running Script "++script) >>
                       log_debug ("HS-"++(show msg))

foreign import ccall send_hs :: CString -> IO ()

send :: String -> ReaderT Fd IO ()
send s = liftIO (newCString s) >>= \cs ->
            liftIO (send_hs cs) >>
            liftIO (free cs)

log_debug :: String -> ReaderT Fd IO ()
log_debug s = ask >>=
              liftIO . (flip fdWrite $ "+40 "++s) >>
              return ()

log_info :: String -> ReaderT Fd IO ()
log_info s = ask >>=
             liftIO . (flip fdWrite $ "+30 "++s) >>
              return ()

log_warn :: String -> ReaderT Fd IO ()
log_warn s = ask >>=
             liftIO . (flip fdWrite $ "+20 "++s) >>
              return ()

log_error :: String -> ReaderT Fd IO ()
log_error s = ask >>=
              liftIO . (flip fdWrite $ "+10 "++s) >>
              return ()

log_fatal :: String -> ReaderT Fd IO ()
log_fatal s = ask >>=
              liftIO . (flip fdWrite $ "+00 "++s) >>
              return ()
