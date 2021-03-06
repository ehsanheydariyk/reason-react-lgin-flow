open Utils;
[%bs.raw {|require('./Signin.css')|}];

type state = {
  email: string,
  password: string,
  rememberMe: bool,
};

type valueName =
  | Email
  | Password;

type action =
  | Change(valueName, string)
  | ChangeRemeber;

let reducer = (action, state) =>
  switch (action) {
  | Change(name, text) =>
    switch (name) {
    | Email => ReasonReact.Update({...state, email: text})
    | Password => ReasonReact.Update({...state, password: text})
    }
  | ChangeRemeber =>
    ReasonReact.Update({...state, rememberMe: !state.rememberMe})
  };

let component = ReasonReact.reducerComponent("Signin");

module Login = [%graphql
  {|
  mutation loginUser($email: String!, $password: String!) {
      loginUser(email: $email, password: $password) {
          access_token
      }
  }
|}
];

module LoginMutation = ReasonApollo.CreateMutation(Login);

let make = (~loginAct, _children) => {
  /* let submit = state => Js.log(state); */
  ...component,
  initialState: () => {email: "", password: "", rememberMe: false},
  reducer,
  render: ({state, send}) =>
    <LoginMutation>
      ...{
           (mutation, {result}) => {
             let loginAction =
               Login.make(~email=state.email, ~password=state.password, ());

             switch (result) {
             | NotCalled => "" |> str
             | Loading => <div />
             | Error(error) => <Antd.Notification.Success message="Error" description={"error##message"} />
             | Data(response) =>
               switch (response##loginUser##access_token) {
               | access_token =>
                 Utils.localSetItem("access_token", access_token);
                 ReasonReact.Router.push("/dashboard");
                 loginAct();
                 <div> {str("first")} </div>;
               | _ => <div> {str("first")} </div>
               }
             | _ => <div> {str("first")} </div>
             };
             <div className="isoSignInPage">
               <div className="isoLoginContentWrapper">
                 <div className="isoLoginContent">
                   <div className="isoLogoWrapper">
                     <Router.Link route=Dashboard([])>
                       {Utils.str("IMMIWORK")}
                     </Router.Link>
                   </div>
                   <div className="isoSignInForm">
                  {switch (result){
                  | Error(error) => <Antd.Alert className="isoInputWrapper" message={error##message} type_="error" />
                  | _ => <div className="isoInputWrapper alert-box" ></div>;
                  }}
                     <form
                       onSubmit={
                         event => {
                           ReactEvent.Synthetic.preventDefault(event);
                           Utils.localSetItem("email", state.email);
                           Utils.localSetItem("password", state.password);
                           mutation(~variables=loginAction##variables, ())
                           |> ignore;
                         }
                       }>
                       <div className="isoInputWrapper">
                         <Antd.Input
                           size="large"
                           placeholder="Email"
                           value={state.email}
                           onChange={
                             event =>
                               send(
                                 Change(
                                   Email,
                                   ReactEvent.Form.target(event)##value,
                                 ),
                               )
                           }
                         />
                       </div>
                       <div className="isoInputWrapper">
                         <Antd.Input
                           size="large"
                           type_="password"
                           placeholder="Password"
                           value={state.password}
                           onChange={
                             event =>
                               send(
                                 Change(
                                   Password,
                                   ReactEvent.Form.target(event)##value,
                                 ),
                               )
                           }
                         />
                       </div>
                       <div className="isoInputWrapper isoLeftRightComponent">
                         <Antd.Checkbox
                           checked={state.rememberMe}
                           onChange={_e => send(ChangeRemeber)}>
                           {ReasonReact.string("Remember me")}
                         </Antd.Checkbox>
                         <div>
                           <Router.Link
                             route=ForgotPass className="isoForgotPass">
                             {ReasonReact.string("Forgot password")}
                           </Router.Link>
                         </div>
                       </div>
                       <div className="isoInputWrapper isoOtherLogin">
                         <Antd.Button
                           loading={result == Loading}
                           type_="primary"
                           htmlType="submit"
                           className="isoLoginButton">
                           {ReasonReact.string("Sign in")}
                         </Antd.Button>
                         <div className="social">
                           <span> {ReasonReact.string("or")} </span>
                         </div>
                         <a
                           href="https://accounts.google.com/o/oauth2/auth?client_id=959585273066-in3mrv049rm7gp1eec79j8ogtsoirmhv.apps.googleusercontent.com&redirect_uri=http://dev.immiwork.com/google/authorize&scope=openid+profile+email&response_type=code">
                           <Antd.Button
                             type_="primary btnGooglePlus"
                             className="isoLoginButton btnGooglePlus">
                             {ReasonReact.string("Sign in with Google")}
                           </Antd.Button>
                         </a>
                       </div>
                     </form>
                     <div className="isoCenterComponent isoHelperWrapper">
                       <Router.Link route=Signup>
                         {
                           ReasonReact.string(
                             "Dont have an account? Create one.",
                           )
                         }
                       </Router.Link>
                     </div>
                   </div>
                 </div>
               </div>
             </div>;
           }
         }
    </LoginMutation>,
};