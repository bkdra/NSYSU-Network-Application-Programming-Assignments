# ZeroMQ Messaging for Many Applications: Titles and Highlights

## Section Titles

### Pub-Sub Message Envelopes
- Pub-sub can split the routing key into a separate message frame called an envelope.
- Using an envelope is optional, but it is cleaner when the key and payload are naturally separate.
- Prefix matching should not cross a frame boundary, which makes envelopes safer for subscriptions.

### High-Water Marks
- High-water marks limit how many messages can queue up before a socket starts applying backpressure.
- PUB and ROUTER sockets drop data when they hit the HWM, while other socket types block.
- In `inproc` transport, sender and receiver share buffers, so the real HWM is the sum of both sides.
- HWM is counted in message parts, not whole messages.

### Missing Message Problem Solver
- Missing messages are a real issue when a peer becomes slow or overloaded.
- The chapter recommends building prototypes and stress-testing them until failure modes are clear.
- The goal is to understand where data can be lost and then design around it with proper flow control.

## Figure Titles

- Figure 2-1. TCP sockets are 1-to-1
- Figure 2-2. HTTP on the wire
- Figure 2-3. ZeroMQ on the wire
- Figure 2-4. Small-scale pub-sub network
- Figure 2-5. Pub-sub network with a proxy
- Figure 2-6. Extended publish-subscribe
- Figure 2-7. Request distribution
- Figure 2-8. Extended request-reply
- Figure 2-9. Request-reply broker
- Figure 2-10. Pub-sub forwarder proxy
- Figure 2-11. Parallel pipeline with kill signaling
- Figure 2-13. The relay race
- Figure 2-14. Pub-sub synchronization
- Figure 2-15. Pub-sub envelope with separate key
- Figure 2-16. Pub-sub envelope with sender address
- Figure 2-17. Missing message problem solver
